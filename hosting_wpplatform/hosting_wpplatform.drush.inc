<?php
/**
 * @file
 * Implement drush hooks for the Platforms module.
 */

/**
 * Implements hook_hosting_TASK_OBJECT_context_options().
 */
function hosting_hosting_wpplatform_context_options(&$task) {
  $task->context_options['server'] = '@server_master';
  $task->context_options['web_server'] = hosting_context_name($task->ref->web_server);
  // Trim the path so we dont pass extra spaces.
  $task->context_options['root'] = trim($task->ref->publish_path, " ");
  if ($task->ref->makefile) {
    $task->context_options['makefile'] = $task->ref->makefile;
  }
  if (isset($task->ref->make_working_copy) && !empty($task->ref->make_working_copy)) {
    $task->context_options['make_working_copy'] = $task->ref->make_working_copy;
  }
  else {
    $task->context_options['make_working_copy'] = 'null';
  }

  if (empty($task->ref->hosting_name)) {
    $task->ref->hosting_name = hosting_context_name($task->ref->nid);
  }
}

/**
 * Implements hook_drush_context_import().
 */
function hosting_wpplatform_drush_context_import($context, &$node) {
  if ($context->type == 'wpplatform') {
    $node->title = str_replace('wpplatform_', '', trim($context->name, '@'));
    $node->web_server = hosting_drush_import($context->web_server);
    $node->publish_path = $context->root;
    $node->makefile = $context->makefile;
    if (isset($context->make_working_copy) && !empty($context->make_working_copy)) {
      $node->make_working_copy = $context->make_working_copy;
    }
  }
}

/**
 * Implements hook_post_verify().
 *
 * Sets the wpplatform verified timestamp, to discren when it was verified.
 * Imports all the profiles and modules into package and package release nodes.
 */
function hosting_wpplatform_post_hosting_verify_task($task, $data) {
  $node = $task->ref;
  if ($node->type == 'wpplatform') {
    // Lock wpplatforms by default
    if ($node->verified == 0 && variable_get('hosting_lock_wpplatforms_by_default', FALSE)) {
      $node->wpplatform_status = HOSTING_WPPLATFORM_LOCKED;
    }

    // Set verified flag on wpplatform, to let it know it has been checked.
    $node->verified = REQUEST_TIME;

    /**
     * If we are verifying a Locked wpplatform (i.e if the publish_path has changed),
     * don't reset the status to Enabled. We don't need to check whether a wpplatform
     * is deleted here for the same reason, because we don't allow a deleted wpplatform
     * to be reverified.
     */
    if ($node->wpplatform_status != HOSTING_WPPLATFORM_LOCKED) {
      $node->wpplatform_status = HOSTING_WPPLATFORM_ENABLED;
    }
    $node->no_verify = TRUE;
    // Save the wpplatform being verified
    node_save($node);
  }
}

/**
 * Implements hook_hosting_post_DELETE().
 */
function hosting_wpplatform_post_hosting_delete_task($task, $data) {
/* [ML] NOT IMPLEMENTED YET. "as-is", this code would run on a normal drupal platform delete.
   We probably need a "wordpress-delete" task.
*/
/*
  $task->ref->wpplatform_status = HOSTING_WPPLATFORM_DELETED;
  $task->ref->no_verify = TRUE;
  node_save($task->ref);
  hosting_context_delete($task->ref->nid);
  db_delete('hosting_wpplatform_client_access')
    ->condition('pid', $task->ref->nid)
    ->execute();
  // Clean up package instances
*/
/* TODO
  hosting_package_instance_sync($task->ref->nid, 'wpplatform');
*/
}

/**
 * @todo Please document this function.
 * @see http://drupal.org/node/1354
 */
function hosting_wpplatform_post_hosting_lock_task($task, $data) {
  $task->ref->wpplatform_status = HOSTING_WPPLATFORM_LOCKED;
  $task->ref->no_verify = TRUE;
  node_save($task->ref);
}

/**
 * @todo Please document this function.
 * @see http://drupal.org/node/1354
 */
function hosting_wpplatform_post_hosting_unlock_task($task, $data) {
  $task->ref->wpplatform_status = HOSTING_WPPLATFORM_ENABLED;
  $task->ref->no_verify = TRUE;
  node_save($task->ref);
}

