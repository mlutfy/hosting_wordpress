<?php
/**
 * @file
 * Hosting site views integration.
 */

/**
 * Implements hook_views_data().
 */
function hosting_site_views_data() {
  // Views data for 'hosting_site' table.
  $data['hosting_site']['table'] = array(
    'group' => 'Hosting Site',
    'title' => 'Site',
    'join' => array(
      'node' => array(
        'left_field' => 'vid',
         'field' => 'vid',
      ),
    ),
  );

  $data['hosting_site']['client'] = array(
   'title' => t('Client'),
   'help' => t('Relate a node revision to the user who created the revision.'),
   'relationship' => array(
      'handler' => 'views_handler_relationship',
      'base' => 'node',
      'base field' => 'nid',
      'label' => t('client'),
    ),
  );

  $data['hosting_site']['db_server'] = array(
   'title' => t('Database Server'),
   'help' => t('Database where the site is installed.'),
   'relationship' => array(
      'handler' => 'views_handler_relationship',
      'base' => 'node',
      'base field' => 'nid',
      'label' => t('db server'),
    ),
  );

  $data['hosting_site']['profile'] = array(
    'title' => t('Install Profile'),
    'help' => t('Type of drupal site.'),
    'relationship' => array(
      'handler' => 'views_handler_relationship',
      'base' => 'node',
      'base field' => 'nid',
      'label' => t('profile'),
    ),
  );

  $data['hosting_site']['platform'] = array(
   'title' => t('Platform'),
   'help' => t('Platform'),
   'relationship' => array(
      'handler' => 'views_handler_relationship',
      'base' => 'node',
      'base field' => 'nid',
      'label' => t('platform'),
    ),
  );

  $data['hosting_site']['verified'] = array(
    'title' => t('Verified'),
    'help' => t('The last date verified task run on this site.'),
    'field' => array(
      'handler' => 'hosting_field_handler_interval',
      'click sortable' => TRUE,
    ),
  );

  $data['hosting_site']['last_cron'] = array(
    'title' => t('Last Cron Run'),
    'help' => t('The time the last cron run was executed on this site.'),
    'field' => array(
      'handler' => 'hosting_field_handler_interval',
      'click sortable' => TRUE,
    ),
  );

  $data['hosting_site']['language'] = array(
    'title' => t('Language'),
    'help' => t('The default language of this site.'),
    'field' => array(
      'handler' => 'hosting_site_handler_field_language',
      'click sortable' => TRUE,
    ),
  );

  $data['hosting_site']['status'] = array(
    'title' => t('Status'),
    'help' => t('The current state of this site.'),
    'field' => array(
      'handler' => 'hosting_site_handler_field_status',
      'click sortable' => TRUE,
    ),
    'filter' => array(
      'handler' => 'views_handler_filter_in_operator',
      'options callback' => 'hosting_site_status_codes',
      'options arguments' => array('label'),
    ),
    'sort' => array(
      'handler' => 'views_handler_sort',
    ),
  );

  // Views data for 'hosting_site_backups' table.
  $data['hosting_site_backups']['table'] = array(
    'title' => 'Backups',
    'group' => 'Hosting Site Backups',
    'base' => array(
      'title' => t('Hosting site backups'),
      'help' => t('Hosting site backups'),
      'field' => 'bid',
    ),
    'join' => array(
      'node' => array(
        'left_field' => 'nid',
         'field' => 'site',
      ),
    ),
  );

  $data['hosting_site_backups']['bid'] = array(
    'title' => t('Bid'),
    'help' => t('The site backup ID.'),
    'field' => array(
      'handler' => 'views_handler_field_numeric',
      'click sortable' => TRUE,
    ),
    'argument' => array(
      'handler' => 'views_handler_argument_numeric',
      'numeric' => TRUE,
    ),
    'filter' => array(
      'handler' => 'views_handler_filter_numeric',
    ),
    'sort' => array(
      'handler' => 'views_handler_sort',
    ),
  );

  $data['hosting_site_backups']['site'] = array(
    'title' => t('Nid'),
    'help' => t('The backups site node ID.'),
    'field' => array(
      'handler' => 'views_handler_field_node',
      'click sortable' => TRUE,
    ),
    'relationship' => array(
      'handler' => 'views_handler_relationship',
      'base' => 'node',
      'base field' => 'nid',
      'label' => t('site'),
    ),
    'argument' => array(
      'handler' => 'views_handler_argument_node_nid',
    ),
  );

  $data['hosting_site_backups']['web_server'] = array(
    'title' => t('Web Server'),
    'help' => t('The web server the backup is hosted on.'),
    'field' => array(
      'handler' => 'views_handler_field_node',
      'click sortable' => TRUE,
    ),
    'relationship' => array(
      'handler' => 'views_handler_relationship',
      'base' => 'node',
      'field' => 'web_server',
      'label' => t('web server'),
    ),
    'argument' => array(
      'handler' => 'views_handler_argument_node_nid',
    ),
  );

  $data['hosting_site_backups']['description'] = array(
    'title' => t('Description'),
    'help' => t('The description of the backup.'),
    'field' => array(
      'handler' => 'views_handler_field',
      'click sortable' => TRUE,
     ),
    'sort' => array(
      'handler' => 'views_handler_sort',
    ),
    'filter' => array(
      'handler' => 'views_handler_filter_string',
    ),
    'argument' => array(
      'handler' => 'views_handler_argument_string',
    ),
  );

  $data['hosting_site_backups']['filename'] = array(
    'title' => t('File'),
    'help' => t('The backup file.'),
    'field' => array(
      'handler' => 'views_handler_field_file',
      'click sortable' => TRUE,
     ),
    'sort' => array(
      'handler' => 'views_handler_sort',
    ),
    'filter' => array(
      'handler' => 'views_handler_filter_string',
    ),
    'argument' => array(
      'handler' => 'views_handler_argument_string',
    ),
  );

  $data['hosting_site_backups']['size'] = array(
    'title' => t('Size'),
    'help' => t('The size of the backup file.'),
    'field' => array(
      'handler' => 'views_handler_field_file_size',
      'click sortable' => TRUE,
     ),
    'sort' => array(
      'handler' => 'views_handler_sort',
    ),
    'filter' => array(
      'handler' => 'views_handler_filter_numeric',
    ),
  );

  $data['hosting_site_backups']['timestamp'] = array(
    'title' => t('Creation date'),
    'help' => t('The date the backup was created.'),
    'field' => array(
      'handler' => 'views_handler_field_date',
      'click sortable' => TRUE,
    ),
    'sort' => array(
      'handler' => 'views_handler_sort_date',
    ),
    'filter' => array(
      'handler' => 'views_handler_filter_date',
    ),
  );

  return $data;
}

/**
 * Implements hook_views_pre_build().
 */
function hosting_site_views_pre_build(&$view) {
  if ($view->name == 'hosting_site_list') {
    switch ($view->current_display) {
      case 'page_sites':
        // Only allow VBO for admins.
        if (!user_access('administer sites')) {
          unset($view->field['views_bulk_operations']);
          unset($view->display_handler->handlers['field']['views_bulk_operations']);
        }
        break;
      case 'block_sites':
        // Rather than maintain separate views/displays for each hosting
        // content type, we alter the view to remove all but the relevant
        // contextual filters (arguments) here.
        // @see: hosting_site_block_view_alter().
        if($node = menu_get_object()) {
          if (in_array($node->type, array('platform', 'server', 'client', 'package'))) {
            foreach ($view->argument as $name => $arg) {
              if (!in_array($node->type, $arg->options['validate_options']['types'])) {
                unset($view->argument[$name]);
                unset($view->display_handler->handlers['argument'][$name]);
                unset($view->display_handler->display->display_options['arguments'][$name]);
                unset($view->display_handler->options['arguments'][$name]);
              }
            }
            // Only allow VBO for admins.
            if (!user_access('administer sites')) {
              unset($view->field['views_bulk_operations']);
              unset($view->display_handler->handlers['field']['views_bulk_operations']);
            }
          }
        }
        break;
    }
  }

}

/**
 * Implements hook_preprocess_views_view().
 */
function hosting_site_preprocess_views_view(&$vars) {
  if ($vars['view']->name == 'hosting_site_list') {
    // Wrap exposed filters in a fieldset.
    if ($vars['exposed']) {
      // Ensure required css and js libs are added
      drupal_add_js('misc/form.js');
      drupal_add_js('misc/collapse.js');

      // Assume other get vars are exposed filters, so expand fieldset
      // to show applied filters
      $class = array('collapsible', 'collapsed');
      if (count($_GET) > 1){
        $class = array('collapsible');
      }

      // Build fieldset element, using correct array nesting for theme_fieldset
      $fieldset['element'] = array(
        '#title' => t('Filters'),
        '#attributes' => array('class' => $class),
        '#value' => $vars['exposed'],
        '#children' => '',
      );

      // Reassign exposed filter tpl var to fieldset value
      $vars['exposed'] = theme('fieldset', $fieldset);
    }
  }
}

/**
 * Callback to collapse VBO fieldset.
 *
 * @see: hosting_site_form_alter().
 */
function _hosting_site_collapse_views_fieldset($form, &$form_state) {
  // Ensure required js libs are added
  drupal_add_js('misc/form.js');
  drupal_add_js('misc/collapse.js');

  $form['select']['#attributes']['class'][] = ' collapsible collapsed';
  return $form;
}
