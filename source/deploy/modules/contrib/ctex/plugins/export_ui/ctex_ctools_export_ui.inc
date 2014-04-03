<?php
/**
 * @file
 * Ctext export UI plugin.
 *
 * @copyright Copyright(c) 2012 Christopher Skene
 * @license GPL v2 http://www.fsf.org/licensing/licenses/gpl.html
 * @author Chris Skene chris at xtfer dot com
 */

/**
 * Define this Export UI plugin.
 */
$plugin = array(
  // As defined in hook_schema().
  'schema' => 'ctex_presets',
  // Define a permission users must have to access these pages.
  'access' => 'administer ctex',

  // Define the menu item.
  'menu' => array(
    'menu item' => 'ctex',
    'menu title' => 'Chaos Tools Export UI Template',
    'menu description' => 'Administer ctex presets.',
  ),

  // Define user interface texts.
  'title singular' => t('preset'),
  'title plural' => t('presets'),
  'title singular proper' => t('Ctex preset'),
  'title plural proper' => t('Ctex presets'),

  // Define the names of the functions that provide the add/edit forms.
  'form' => array(
    'settings' => 'ctex_ctools_export_ui_form',
    // 'submit' and 'validate' are also valid callbacks.
  ),
);

/**
 * Implements hook_ctools_export_ui_form().
 */
function ctex_ctools_export_ui_form(&$form, &$form_state) {
  $preset = $form_state['item'];

  $form['field_one'] = array(
    '#type' => 'textfield',
    '#title' => t('Field one'),
    '#description' => t('The first field from your schema.'),
    '#default_value' => $preset->field_one,
    '#required' => TRUE,
  );

  // Add Mymodule's configuration interface.
  $form['field_two'] = array(
    '#type' => 'textfield',
    '#title' => t('Field two'),
    '#description' => t('The second field from your scheme.'),
    '#default_value' => $preset->field_two,
    '#required' => TRUE,
  );
}