<?php
/**
 * @file
 * Utility functions and page callbacks for content_staging module.
 *
 * @todo configurable version control checkout script
 * @todo make checkout before update optional
 * @todo allow checkout script to be configurable (e.g. to use Git rather than subversion)
 * @todo only process files with newer build than on website (to save time)
 * @todo allow file types other than .txt ?
 * @todo update on cron
 * @todo Allow to work with Git using gitattribute Keyword Expansion and smudge: http://progit.org/book/ch7-2.html
 */

/**
 * Run Writeup on a file
 *
 * @param $file
 *   path of file to be processed
 *
 * @param $options
 *   options to be passed to Writeup
 *
 * @param &$output
 *   array containing output from Writeup process (passed by reference)
 *
 * @param $see_errors
 *   Include error message sent to stdout (to supress normal output and just see errors, use -x NULL)
 *
 * @return
 *   passes the Writeup return code
 */
function _cs_writeup_exec($file, $options, &$output, $see_errors=FALSE) {
  $stderr = $see_errors ? ' 2>&1' : '';
  $writeup = rtrim(variable_get('writeup_loc', ''), '/\\') . '/writeup';
  $options .= ' --set STAGING_DIR=/' . variable_get('content_staging_dir', 'staging');
  $retvar = 0;
  exec( $writeup . ' -ybqrf drupal ' . $options . ' ' . $file . $stderr, $output, $retvar ); //run quietly, redirecting to stdout
  return ($retvar);
}

/**
 * Build array of keywords extracted from a file
 *
 * @param $file
 *   path of file to be processed
 *
 * @param &$keywords
 *   array containing keywords and values
 *
 * @return
 *   passes the Writeup return code
 */
function writeup_keywords($file, &$keywords=array()) {
  $output=array();
  $keywords['RETCODE'] = _cs_writeup_exec($file, '-X "*"', $output);
  foreach ($output as $line) {
    $equals = strpos($line, '=');
    $keywords[substr($line, 0, $equals)] = substr($line, $equals+2, -1); //do not use drupal_substr here
  }
  return ($keywords['RETCODE']);
}

/**
 * Get date/time string from a keywords array
 *
 * @param $keywords
 *   array of keywords
 *
 * @return
 *   Date and time in format: yyyy-mm-dd hh:mm:ss
 */
function _cs_get_date($keywords) {
  // if the version control system has set a date, use that. Else use the file's date
  $filedate = isset($keywords['SVNDATE']) ? $keywords['SVNDATE'] : $keywords['DATE'];
  $filetime = isset($keywords['SVNTIME']) ? $keywords['SVNTIME'] : $keywords['TIME'];
  return $filedate . ' ' . $filetime;
}

/**
 * Get build number of a document. If no SVN, use datestamp
 *
 * @param $keywords
 *   array of keywords returned by writeup_keywords
 *
 * @return
 *   build number, or datestamp
 */
function _cs_getDocBuild($keywords) {
  if (isset($keywords['SVNBUILD'])) return $keywords['SVNBUILD'];
  else return strtotime(_cs_get_date($keywords));
}

/**
 * Get filename from path, including path offset from the root, but not the root name
 * e.g. pub/subdir/filename.txt -> subdir/filename
 *
 * @param $path
 *   full path
 *
 * @return
 *   filename including partial path, but no file extension
 */
function _cs_getName($path) {
  $rootlen = strlen(variable_get('content_staging_dir', '')) + 1; // plus 1 for the '/' separator (do not use drupal_strlen here)
  return substr($path, $rootlen, -4); // -4 for '.txt' This will have to be changed if other extensions allowed (do not use drupal_substr here)
}

/**
 * Get build number of Drupal node
 * The node_revision table is used to store the build number
 *
 * @param $nid
 *   node id
 *
 * @return
 *   current build number of node
 */
function _cs_getNodeBuild($nid) {
  return _cs_db_result(SQL_REVISION, array(":d" => $nid));
}

/**
 * Get the Node ID corresponding with a path, and therefore a URL alias
 *
 * @param $path
 *   path
 *
 * @return
 *   node id or -1 if not found
 */
function _cs_getNid($path) {
  $source = _cs_db_result(SQL_ALIAS, array(":s" => _cs_getName($path)));
  if ($source == FALSE) return -1;
  else return (int)substr($source, 5); // strip out 'node/' (do not use drupal_substr here)
}

/**
 * Generate a list of all files in a directory and all subdirectories that have a given file extension.
 *
 * @param $root_dir
 *   root directory for the list, with no trailing '/'
 *
 * @param $allowext
 *   array of permissible file extensions
 *
 * @param $all_data
 *   resulting array, passed by reference
 *
 * @return
 *   resulting array
 */
function _cs_scan_directories($root_dir, $allowext, &$all_data = array()) {
  $dir_content = scandir($root_dir);
  foreach ($dir_content as $key => $content) {
    if ($content == '.' || $content == '..') continue;
    $path = $root_dir . DIRECTORY_SEPARATOR . $content;
    $ext = substr($content, strrpos($content, '.') + 1); // do not use drupal_substr here
    if (in_array($ext, $allowext) && is_file($path) && is_readable($path)) {
      $all_data[] = $path;
    }
    elseif (is_dir($path) && is_readable($path)) {
      // recursive callback to open new directory
      _cs_scan_directories($path, $allowext, $all_data);
    }
    // else unreadable garbage
  }
  return $all_data;
}

/**
 * Generate a table of all staged files, showing their status.
 *
 * @return
 *   resulting page
 */
function content_staging_file_status_page() {
  $page = '<h2>' . t('List of files in content staging') . '</h2>'
        . '<p>' . t('To update files, go to the !url.', array('!url' => l(t('update page'), 'cs_update'))) . '</p>';
  $dir = variable_get('content_staging_dir', '');
  if ($dir == '') return t('Error: Content staging directory is not yet set up. Please go to the !url and configure it.', array('!url' => l(t('admin page'), ADMIN_CS)));
  if (variable_get('content_staging_runstatus', 1) == 1) {
    $update   = variable_get('content_staging_update', 'echo "no update command set for directory:"');
    $lastline = exec( $update . ' ' . $dir);
    $page .= '<pre>' . $lastline . '</pre><hr />';
  }
  $files = _cs_scan_directories($dir, array("txt"));
  $page .='<table><tr><th>' . t('Filename') . '</th><th>' . t('Type') . '</th><th>'
          . t('File ver') . '</th><th>' . t('Node ver') . '</th><th>nid</th><th>'
          . t('Timestamp') . '</th><th>' . t('Title') . '</th><tr>';
  foreach ($files as $file) {
    if (substr($file, -4) != '.txt') continue; // do not use drupal_substr here
    $keywords = array();
    writeup_keywords($file, $keywords);
    if (!isset($keywords['CS_UPDATE']) || $keywords['CS_UPDATE'] != 'TRUE') continue; // file not marked to update
    $nid = _cs_getNid($file);
    $page .= '<tr><td>' . l(_cs_getName($file), 'cs_listvars/' . _cs_getName($file));
    $page .= '</td><td>' . $keywords['TYPE'];
    $page .= '</td><td>' . _cs_getDocBuild($keywords);
    $page .= '</td><td>' . _cs_getNodeBuild($nid);
    $page .= '</td><td>' . l($nid, 'cs_listnode/' . $nid);
    $page .= '</td><td>' . _cs_get_date($keywords);
    $page .= '</td><td>';
    if ($keywords['RETCODE'] != 0) {
      $page .= l($keywords['RETCODE'] == 1 ? t('Warning') : t('Error'),
        'cs_listerr/' . _cs_getName($file),
        array('attributes' => array('style' => 'color:red; font-weight:bold;'))) . ' ';
    }
    $page .= l($keywords['TITLE'], _cs_getName($file));
    $page .= '</td></tr>';
  }
  $page .= '</table>';
  return t($page);
}

/**
 * Update node from file
 *
 * @param $file
 *   filename (corresponds with URL alias of target node)
 *
 * @return
 *   result to be displayed in update status log page
 */
function content_staging_update_node($file) {
  $keywords = array();
  writeup_keywords($file, $keywords);
  if (!isset($keywords['CS_UPDATE']) || $keywords['CS_UPDATE'] != 'TRUE') return ''; // file not marked to update
  $nid = _cs_getNid($file);
  if (($nid == -1) || ($keywords['RETCODE'] != 0) || ($keywords['TYPE'] != 'writeup')) return '';
  $log = '<tr><td>' . $file . '</td><td>' . $nid . '</td><td>' . l($keywords['TITLE'], _cs_getName($file)) . '</td><td>';

  // make changes to node using node_load and node_save --slow but accurate
  $node = node_load($nid);
  if ($node == FALSE ) return $log . t('Invalid node') . '</td></tr>';

  // update node object
  $node->title = $keywords['TITLE'];
  if (isset($keywords['CS_PUBLISH'])) $node->status  = $keywords['CS_PUBLISH'];
  if (isset($keywords['CS_PROMOTE'])) $node->promote = $keywords['CS_PROMOTE'];
  if (isset($keywords['CS_STICKY']))  $node->sticky  = $keywords['CS_STICKY'];

  // process body and put it into database
  $output=array();
  _cs_writeup_exec($file, '', $output);
  _cs_node_body($node, implode("\n", $output));
  $node->log = _cs_getDocBuild($keywords); // put version/build number in log
  $node = node_submit($node);
  node_save($node);
  $svntimestamp = strtotime(_cs_get_date($keywords));  // have to manually set timestamps
  $sql = "UPDATE {node} SET changed=%d, created=%d WHERE nid=%d;";
  _cs_db_update('node', array('changed' => $svntimestamp, 'created' => $svntimestamp), array('nid', $nid, '='));
  return $log . '<em>' . t('updated') . '</em></td></tr>';
}

/**
 * Update all nodes that have associated files
 *
 * @return
 *   resulting update status log page
 */
function content_staging_update_page() {
  $page = '<h2>Updating content from staging</h2>';
  $dir   = variable_get('content_staging_dir', 'staging');
  if (variable_get('content_staging_runupdate', 0) == 1) {
    $update   = variable_get('content_staging_update', 'echo "no update command set for directory:"');
    $lastline = exec( $update . ' ' . $dir);
    $page .= '<pre>' . $lastline . '</pre><hr />';
  }
  $files = _cs_scan_directories($dir, array("txt")); //find all the files
  $page .= '<table><tr><th>File</th><th>nid</th><th>Title</th><th>Result</th></tr>';
  foreach ($files as $file) {
    if ( (substr($file, -4) != '.txt')) continue; // do not use drupal_substr here
    $page .= content_staging_update_node($file);
  }
  $page .= '</table>';
  return t($page);
}

/**
 * List variables in a file for diagnostic purposes
 *
 * @param $file
 *   file path
 *
 * @return
 *   page containing file variables printed out with print_r
 */
function content_staging_list_variables($file = '') {
  if ($file == '') return '<p>' . t('No file specified') . '</p>';
  $path = variable_get('content_staging_dir', 'staging');
  for ($i = 1; !is_null(arg($i)); $i++) $path .= '/' . arg($i); // build path from arguments
  $path .= '.txt';
  if (!file_exists($path)) return '<p>' . t('Path does not exist: @path', array('@path' => $path)) . '</p>';
  $page = '<h2>Listing variables in file ' . $path . '</h2>';
  $keywords=array();
  writeup_keywords($path, $keywords) . "<br />";
  return $page . '<pre>' . print_r($keywords, TRUE) . '</pre>';
}

/**
 * List error messages from a file
 *
 * @param $file
 *   file path
 *
 * @return
 *   page containing error message text (if any)
 */
function content_staging_file_errors($file = '') {
  if ($file == '') return '<p>' . t('No file specified') . '</p>';
  $path = variable_get('content_staging_dir', 'staging');
  for ($i = 1; !is_null(arg($i)); $i++) $path .= '/' . arg($i); // build path from arguments
  $path .= '.txt';
  if (!file_exists($path)) return '<p>' . t('Path does not exist: @path', array('@path' => $path)) . '</p>';
  $output=array();
  _cs_writeup_exec($path, '-x NULL', $output, TRUE);
  return '<h2>Errors in file: ' . $path . '</h2><pre>' . implode("\n", $output) . '</pre>';
}

/**
 * List contents of a node for diagnostic purposes
 *
 * @param $nid
 *   node id
 *
 * @return
 *   page containing node printed out with print_r
 */
function content_staging_list_node($nid = '-1') {
  if ($nid == -1) return t('No node specified');
  $node = node_load($nid);
  if ($node == FALSE ) return t('Invalid node specified');
  $page = '<h2>Listing contents of node ' . $nid . '</h2>';
  $page .= 'title: ' . $node->title . '<br />';
  return $page . '<pre>' . print_r($node, TRUE) . '</pre>';
}


