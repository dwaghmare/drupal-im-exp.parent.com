<?php
/**
 * @file
 * Provides info-type hook implementations that are infrequently called.
 *
 * @author Jim Berry ("solotandem", http://drupal.org/user/240748)
 */

/**
 * Implements hook_menu().
 */
function _portable_path_menu() {
  $items['admin/config/content/portable'] = array(
    'title' => 'Portable path',
    'description' => 'Configure which fields and paths are converted to portable paths before saving an entity.',
    'page callback' => 'drupal_get_form',
    'page arguments' => array('portable_path_settings_form'),
    'access arguments' => array('administer filters'),
    'file' => 'includes/settings.inc',
  );
  return $items;
}

/**
 * Implements hook_filter_info().
 *
 * Added custom key of "encode callback."
 */
function _portable_path_filter_info() {
  $filters['file_id'] = array(
    'title' => t('Convert file ID tokens to relative URLs'),
    'description' => t('Replaces a token in file ID format (i.e., "[portable:fid:style]") with the full relative URL.'),
    'process callback'  => 'portable_path_file_id_process',
//     'settings callback' => 'portable_path_file_id_settings',
    'tips callback' => 'portable_path_file_id_tips',
    'encode callback'  => 'portable_path_file_id_encode',
    'default settings' => array(),
    'weight' => -30,
  );
  $filters['stream_wrapper'] = array(
    'title' => t('Convert stream wrapper URLs to relative URLs'),
    'description' => t('Replaces a URL in a local stream wrapper format (e.g. "public://") with the full relative URL.'),
    'process callback'  => 'portable_path_stream_wrapper_process',
//     'settings callback' => 'portable_path_stream_wrapper_settings',
    'tips callback' => 'portable_path_stream_wrapper_tips',
    'encode callback'  => 'portable_path_stream_wrapper_encode',
    'default settings' => array(
      'stream_wrappers' => array(),
    ),
    'weight' => -29,
  );
  return $filters;
}
