<?php
/**
 * @file
 * Hosting platform views integration.
 */

/**
 * Implements hook_views_data().
 */
function hosting_wpplatform_views_data() {
  $data['hosting_wpplatform']['table'] = array(
    'group' => 'Hosting WP Platform',
    'title' => 'WP Platform',
    'join' => array(
      'node' => array(
        'left_field' => 'vid',
        'field' => 'vid',
      ),
    ),
  );

  $data['hosting_wpplatform']['web_server'] = array(
    'title' => t('Web Server'),
    'help' => t('Relate a platform to the web server it is hosted on.'),
    'relationship' => array(
      'handler' => 'views_handler_relationship',
      'base' => 'node',
      'field' => 'web_server',
      'label' => t('web server'),
    ),
  );

  $data['hosting_wpplatform']['publish_path'] = array(
    'title' => t('Publish Path'),
    'help' => t('The path on the server where this platform is installed.'),
    'field' => array(
      'handler' => 'views_handler_field_xss',
      'click sortable' => TRUE,
    ),
  );

  $data['hosting_wpplatform']['release'] = array(
    'title' => t('Release'),
    'help' => t('The release name.'),
    'field' => array(
      'field' => 'nid',
      'handler' => 'hosting_wpplatform_handler_field_release',
      'click sortable' => TRUE,
    ),
  );

  $data['hosting_wpplatform']['verified'] = array(
    'title' => t('Verified Date'),
    'help' => t('The most recent date that this platform was verified.'),
    'field' => array(
      'handler' => 'views_handler_field_date',
      'click sortable' => TRUE,
    ),
  );

  $data['hosting_wpplatform']['status'] = array(
    'title' => t('Status'),
    'help' => t('The current state of this platform.'),
    'field' => array(
      'handler' => 'hosting_wpplatform_handler_field_status',
      'click sortable' => TRUE,
    ),
    'filter' => array(
      'handler' => 'views_handler_filter_numeric',
    ),
  );

  $data['hosting_wpplatform']['sites'] = array(
    'title' => t('Sites'),
    'help' => t('The number of sites in this platform.'),
    'field' => array(
      'handler' => 'hosting_wpplatform_handler_field_sites',
      'field' => 'nid',
    ),
  );

  return $data;
}
