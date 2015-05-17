<?php

/**
 * @file
 * Site nodeapi implementations.
 */

/**
 * Implements hook_node_info().
 */
function hosting_wpsite_node_info() {
  $types["wpsite"] = array(
    "type" => 'wpsite',
    "name" => 'Site',
    'base' => 'hosting_wpsite',
    "has_title" => TRUE,
    "title_label" => 'Domain name',
    "description" => 'FIXME WPSITE description', // hosting_node_help("wpsite"),
    "has_body" => 0,
    "body_label" => '',
    "min_word_count" => 0,
  );
  return $types;
}

/**
 * Implements hook_view().
 */
function hosting_wpsite_view(&$node, $teaser = FALSE) {
  hosting_set_breadcrumb($node);
  $node->content['info']['#prefix'] = '<div id="hosting-wpsite-info" class="hosting-info-list">';
  if ($node->wpsite_status == HOSTING_SITE_ENABLED) {
    $node->content['info']['link'] = array(
      '#markup' => _hosting_wpsite_goto_link($node),
      '#weight' => -10,
    );
  }

  if (is_numeric($node->client)) {
    $node->content['info']['client'] = array(
      '#type' => 'item',
      '#title' => t('Client'),
      '#markup' => _hosting_node_link($node->client),
      '#weight' => 5,
    );
  }

  $node->content['info']['verified'] = array(
    '#type' => 'item',
    '#title' => t('Verified'),
    '#markup' => hosting_format_interval($node->verified),
  );

  $node->content['info']['wpplatform'] = array(
    '#type' => 'item',
    '#title' => t('Platform'),
    '#markup' => _hosting_node_link($node->wpplatform),
  );

  if ($node->profile) {
    $node->content['info']['profile'] = array(
      '#type' => 'item',
      '#title' => t('Install profile'),
      '#markup' => _hosting_node_link($node->profile),
    );
  }
  if ($node->wpsite_language) {
    $node->content['info']['wpsite_language'] = array(
      '#type' => 'item',
      '#title' => t('Language'),
      '#markup' => _hosting_language_name($node->wpsite_language),
    );
  }

  if ($node->nid) {
    $node->content['info']['status'] = array(
      '#type' => 'item',
      '#title' => t('Status'),
      '#markup' => _hosting_wpsite_status($node),
    );
  }

  if ($node->db_server) {
    $node->content['info']['db_server'] = array(
      '#type' => 'item',
      '#title' => t('Database server'),
      '#markup' => _hosting_node_link($node->db_server),
    );
  }

  if ($node->db_name) {
    $node->content['info']['db_name'] = array(
      '#type' => 'item',
      '#title' => t('Database name'),
      '#markup' => check_plain($node->db_name),
    );
  }

  $node->content['info']['#suffix'] = '</div>';

  if ($node->nid) {
    $node->content['tasks_view'] = array(
      '#type' => 'item',
      '#markup' => hosting_task_table($node),
      '#prefix' => '<div id="hosting-task-list">',
      '#suffix' => '</div>',
      '#weight' => 10,
    );
    $settings['hostingTaskRefresh'] = array(
      'nid' => $node->nid,
      'changed' => $node->changed,
    );
    drupal_add_js($settings, array('type' => 'setting', 'scope' => JS_DEFAULT));

    drupal_add_js(drupal_get_path('module', 'hosting_task') . '/hosting_task.js');
  }
  return $node;
}

/**
 * Implements hook_nodeapi_delete_revision().
 */
function hosting_nodeapi_wpsite_delete_revision(&$node) {
  db_delete('hosting_wpsite')
    ->condition('vid', $node->vid)
    ->execute();
}

/**
 * Implements hook_delete().
 */
function hosting_wpsite_delete($node) {
  db_delete('hosting_wpsite')
    ->condition('nid', $node->nid)
    ->execute();
  db_delete('hosting_package_instance')
    ->condition('rid', $node->nid)
    ->execute();
  hosting_context_delete($node->nid);
  hosting_task_delete_related_tasks($node->nid);
}

/**
 * Implements hook_nodeapi().
 */
function hosting_wpsite_nodeapi_wpsite_presave(&$node) {
  // Domain names are case-insensitive.
  $node->title = strtolower(trim($node->title));
}

/**
 * Implements hook_insert().
 */
function hosting_wpsite_insert(&$node) {
  $client = hosting_get_client($node->client);
  $node->client = $client->nid;
  $node->wpsite_language = isset($node->wpsite_language) ? $node->wpsite_language : 'en';
  // If the cron_key is set use it, otherwise generate a new one.
  $node->cron_key = isset($node->cron_key) ? $node->cron_key : '';
  // Ensure that the last_cron value is set.
  $node->last_cron = isset($node->last_cron) ? $node->last_cron : 0;
  // Provide a dummy profile, e.g. for hosting-import.
  $node->profile = isset($node->profile) ? $node->profile : 0;

  $id = db_insert('hosting_wpsite')
    ->fields(array(
      'vid' => $node->vid,
      'nid' => $node->nid,
      'client' => $node->client,
      'db_server' => $node->db_server,
      'db_name' => isset($node->db_name) ? $node->db_name : '',
      'wpplatform' => $node->wpplatform,
      // 'profile' => $node->profile,
      'language' => $node->wpsite_language,
      'last_cron' => $node->last_cron,
      'cron_key' => $node->cron_key,
      'status' => isset($node->wpsite_status) ? $node->wpsite_status : HOSTING_SITE_QUEUED,
      'verified' => isset($node->verified) ? $node->verified : 0,
    ))
    ->execute();

}

/**
 * Implements hook_node_insert().
 */
function hosting_wpsite_node_insert($node) {
  if ($node->type == "wpsite") {
    if ((empty($node->old_vid))) {
      hosting_context_register($node->nid, isset($node->hosting_name) ? $node->hosting_name : $node->title);
      if (isset($node->import)) {
        hosting_add_task($node->nid, 'import');
      }
      else {
        hosting_add_task($node->nid, 'wordpress-install');
      }
    }
  }
}

/**
 * Implements hook_update().
 */
function hosting_wpsite_update(&$node) {
  // If this is a new node or we're adding a new revision.
  if (!empty($node->revision)) {
    hosting_wpsite_insert($node);
  }
  else {
    $client = hosting_get_client($node->client);
    $node->client = $client->nid;
    if ($node->wpsite_status == HOSTING_SITE_DELETED) {
      $node->no_verify = TRUE;
    }

    db_update('hosting_wpsite')
      ->fields(array(
        'client' => $node->client,
        'db_server' => $node->db_server,
        'db_name' => $node->db_name,
        'wpplatform' => $node->wpplatform,
        'last_cron' => $node->last_cron,
        'cron_key' => $node->cron_key,
        'status' => $node->wpsite_status,
        'profile' => $node->profile,
        'language' => $node->wpsite_language,
        'verified' => $node->verified,
      ))
      ->condition('vid', $node->vid)
      ->execute();
  }
}

/**
 * Implements hook_node_update().
 */
function hosting_wpsite_node_update($node) {
  if ($node->type == "wpsite") {
    if ((isset($node->no_verify) && $node->no_verify) == FALSE) {
      hosting_add_task($node->nid, 'verify');
    }
  }
}

/**
 * Implements hook_load().
 */
function hosting_wpsite_load($nodes) {
  foreach ($nodes as $nid => &$node) {
    $additions = db_query('SELECT  client, db_server, db_name, wpplatform, profile, language as wpsite_language, last_cron, cron_key, status AS wpsite_status, verified FROM {hosting_wpsite} WHERE vid = :vid', array(':vid' => $node->vid))->fetch();
    foreach ($additions as $property => &$value) {
      $node->$property = $value;
    }
  }
}
