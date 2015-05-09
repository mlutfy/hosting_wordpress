<?php
/**
 * @file
 * Site backup functions.
 */

/**
 * Confirmation form for site backups.
 *
 * @see hosting_task_confirm_form().
 */
function hosting_task_backup_form($node) {
  $form['description'] = array(
    '#title' => t('Description'),
    '#type' => 'textfield',
    '#required' => FALSE,
    '#weight' => '-1',
    '#description' => "Describe the reasons for creating this backup.",
  );

  return $form;
}

/**
 * Add a site backup record.
 *
 * Builds a list of backups of the site that have been generated.
 */
function hosting_site_add_backup($site, $web_server, $filename, $description = '', $size = '') {
  $id = db_insert('hosting_site_backups')
  ->fields(array(
    'site' => $site,
    'web_server' => $web_server,
    'filename' => $filename,
    'description' => $description,
    'size' => $size,
    'timestamp' => REQUEST_TIME,
  ))
  ->execute();
  return $id;
}

/**
 * Delete a site backup record
 */
function hosting_site_delete_backup($bid) {
  db_delete('hosting_site_backups')
    ->condition('bid', $bid)
    ->execute();
}

/**
 * Get a site backup record
 */
function hosting_site_get_backup($bid) {
  return db_query("SELECT bid, site, web_server, filename, description, size, timestamp FROM {hosting_site_backups} WHERE bid = :bid", array(':bid' => $bid))->fetchAssoc();
}

/**
 * Retrieve a list of backup generated for a site.
 *
 * @param site
 *   The node if of the site backups are being retrieved for
 * @return
 *   An associative array of backups existing for the site, indexed by bid and sorted reverse chronologically.
 */
function hosting_site_backup_list($site) {
  $result = db_query("SELECT bid, description, size, timestamp FROM {hosting_site_backups} WHERE site = :site ORDER BY timestamp DESC", array(':site' => $site));
  $backup = array();
  foreach ($result as $object) {
    #needs to be cleaned up. but i am NOT generating a theme func for this right now.
    $backups[$object->bid] = '<strong>' . format_date($object->timestamp) . '</strong> - ' . format_size($object->size) . ' - ' . filter_xss($object->description);
  }
  return $backups;
}

/**
 * Implements hosting_task_TASK_TYPE_form_validate().
 */
function hosting_task_backup_delete_form_validate($form, &$form_state) {
  if ($form['parameters']['no_backups']) {
    form_set_error('no_backups', t('There are no valid backups available.'));
  }
  if (!$form['parameters']['#post']['parameters']) {
    form_set_error('no_backups', t('No backups were selected for deletion.'));
  }
}

/**
 * Implements hosting_task_TASK_TYPE_form().
 */
function hosting_task_backup_delete_form($node) {
  $list = hosting_site_backup_list($node->nid);
  if (sizeof($list)) {
    foreach ($list as $bid => $info) {
      $backup = hosting_site_get_backup($bid);
      $form[$bid] = array(
        '#type' => 'checkbox',
        '#title' => $info,
        '#return_value' => $backup['filename'],
      );
    }
  }
  else {
    $form['no_backups'] = array(
      '#type' => 'item',
      '#title' => t('Backups'),
      '#value' => t('There are no valid backups available.')
    );
  }
  return $form;
}


