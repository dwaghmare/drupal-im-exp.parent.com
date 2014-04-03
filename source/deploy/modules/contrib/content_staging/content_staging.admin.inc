<?php
/**
 * @file
 * Admin menus and callbacks for the content_staging module.
 * The value of ADMIN_CS is set in the content_staging module to allow this file to be used
 * unchanged in both Drupal 6 and Drupal 7.
 */

/**
 * Implements hook_help().
 * Display help and module information
 *
 * @param path
 *   which path of the site we're displaying help
 *
 * @param arg
 *   array that holds the current path as would be returned from arg() function
 *
 * @return
 *   help text for the path
 */
function content_staging_help($path = 'admin/help#content_staging', $arg) {
  $output = ''; // declare output variable
  switch ($path) {
    case "admin/help#content_staging":
      $output = '<p>' . t("System for pulling content from a staging repository") . '</p>';
      break;
  }
  return $output;
}

/**
 * Implements hook_menu().
 */
function content_staging_menu() {
  $items = array();
  $items[ADMIN_CS] = array( // value of 'ADMIN_CS' defined as a constant
    'title' => 'Content Staging',
    'description' => 'Administer Content Staging',
    'page callback' => 'drupal_get_form',
    'page arguments' => array('content_staging_admin_settings'),
    'access arguments' => array('administer Content Staging settings'),
    'type' => MENU_NORMAL_ITEM,
  );
  $items['cs_status'] = array(
    'title' => 'Content status listing',
    'description' => 'Lists the status of the content',
    'page callback' => 'content_staging_file_status_page',
    'access arguments' => array('view content status'),
    'type' => MENU_CALLBACK,
  );
  $items['cs_update'] = array(
    'title' => 'Update content from repository',
    'description' => 'Updates the content on all the pages in the repository',
    'page callback' => 'content_staging_update_page',
    'access arguments' => array('update content from staging'),
    'type' => MENU_CALLBACK,
  );
  $items['cs_listvars'] = array(
    'title' => 'List of variables in file',
    'description' => 'List of variables in a file (for diagnostic purposes)',
    'page callback' => 'content_staging_list_variables',
    'access arguments' => array('view content status'),
    'type' => MENU_CALLBACK,
  );
  $items['cs_listerr'] = array(
    'title' => 'List of error messages from a file',
    'description' => 'List writeup error messages when processing a file',
    'page callback' => 'content_staging_file_errors',
    'access arguments' => array('view content status'),
    'type' => MENU_CALLBACK,
  );
  $items['cs_listnode'] = array(
    'title' => 'List of node contents',
    'description' => 'List contents of a node array for diagnostic purposes',
    'page callback' => 'content_staging_list_node',
    'access arguments' => array('view content status'),
    'type' => MENU_CALLBACK,
  );
  return $items;
}

/**
 * Administration form.
 */
function content_staging_admin_settings() {
  $form['content_staging_admin'] = array(
    '#type' => 'fieldset',
    '#title' => t('Configuration Settings for Content Staging'),
    '#description' => '<ul><li>'
     . t('To view the status of the files in staging, see the !url.', array('!url' => l(t('status page'), 'cs_status'))) . '</li><li>'
     . t('To update the website content from  staging, !url.', array('!url' => l(t('run update'), 'cs_update'))) . '</li></ul>',
    '#collapsible' => TRUE,
    '#collapsed' => FALSE,
  );
  $form['content_staging_admin']["writeup_loc"] = array(
    '#type' => 'textfield',
    '#title' => t('Location for Writeup binary'), // Note that this location is shared with the Writeup module (input filter)
    '#description' => t('The directory containing the Writeup executable binary, e.g. /opt/writeup')
       . '<br />' . t('Found version:') . ' ' . _content_staging_version(),
    '#default_value' => variable_get('writeup_loc', ''),
    '#after_build' => array('content_staging_check_directory'),
    '#size' => 70,
  );
  $form['content_staging_admin']["content_staging_dir"] = array(
    '#type' => 'textfield',
    '#title' => t('Top level directory for content'),
    '#description' => '<p>' . t('The top level directory of the tree containing the staged content.') . '</p><ul><li>'
      . t('If the directory begins with / then it will be absolute within the server file system.') . '</li><li>'
      . t('Otherwise it will be relative to the root directory of the web site.') . '</li><li>'
      . t('E.g. if it is the /staging directory of the website, then use the value: staging.') . '</li></ul><p>'
      . t('It is suggested that this be somewhere accessible to your server, so images can be served from directly within the tree.') . '</p>',
    '#default_value' => variable_get('content_staging_dir', 'staging'),
    '#after_build' => array('content_staging_check_directory'),
    '#size' => 70,
  );
  $form['content_staging_admin']['content_staging_runstatus'] = array(
    '#type' => 'checkbox',
    '#title' => t('Run server update command on status page'),
    '#description' => t('Run the following server command before showing status content.'),
    '#default_value' => variable_get('content_staging_runstatus', 1),
  );
  $form['content_staging_admin']['content_staging_runupdate'] = array(
    '#type' => 'checkbox',
    '#title' => t('Run server update command before updating content'),
    '#description' => t('Run the following server command before updating content.'),
    '#default_value' => variable_get('content_staging_runupdate', 0),
  );
  $form['content_staging_admin']["content_staging_update"] = array(
    '#type' => 'textfield',
    '#title' => t('Script to run to update content'),
    '#description' => t('This script is run using the above directory as an argument.')
       . '<br />' . t('e.g. svn up --config-dir /var/www/html')
       . '<br />' . t('Note that in many cases is it necessary to set up a config directory containing authentication data.'),
    '#default_value' => variable_get('content_staging_update', 'echo "no update command set for directory:"'),
    '#size' => 70,
  );
  $form['content_staging_admin']["content_staging_textformat"] = array(
    '#type' => 'textfield',
    '#title' => t('Text Format to use (machine name)'),
    '#description' => t('E.g. full_html. (Leave blank to leave original format of node intact)')
       . '<br />' . t('For full control of content it is recommended to create a text format called Raw which lets everything through, and then make sure that access to the content staging version control system is properly secure.'),
    '#default_value' => variable_get('content_staging_textformat', ''),
    '#size' => 70,
  );
  return system_settings_form($form);
}

/**
 * Checks the existence of the directory specified in $form_element.
 * If validation fails, the form element is flagged.
 *
 * @param $form_element
 *   The form element containing the name of the directory to check.
 */
function content_staging_check_directory($form_element) {
  $directory = rtrim($form_element['#value'], '/\\');
  if (!is_dir($directory)) {   // Check if directory exists.
    form_set_error($form_element['#parents'][0], t('The directory %directory does not exist.', array('%directory' => $directory)));
  }
  return $form_element;
}

/**
 * Returns version of Writeup binary
 *
 * @return
 *   version of Writeup binary, or error message
 */
function _content_staging_version() {
  $versionmin = 1.99;
  $err = ' <span style="color:red;font-weight:bold;">';
  $errend = '</span>';
  $writeup = rtrim(variable_get('writeup_loc', ''), '/\\') . '/writeup';
  if (!is_file($writeup)) {   // Check if Writeup binary exists.
    $msg =  $err . t('No Writeup binary executable was found in this folder.') . $errend;
  }
  else {
    $msg = shell_exec("$writeup --version");
    if ($versionmin > shell_exec("$writeup --versionnum")) {
      $msg .= $err
      . t('Error: module requires a minimum of version %versionmin of the Writeup binary.', array('%versionmin' => $versionmin))
      . $errend;
    }
  }
  return $msg;
}

