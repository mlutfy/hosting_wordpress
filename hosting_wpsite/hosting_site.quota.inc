<?php
/**
 * @file
 * Quota-relaled hooks for the Hosting site module.
 */

/**
 * Implements hook_hosting_quota_resource().
 */
function hosting_site_hosting_quota_resource() {
  $resources = array();

  $resources['sites'] = array(
    'title' => t('Sites'),
    'description' => t('The number of sites this client can have provisioned under them at any one time. Set to 0 for unlimited.'),
    'module' => 'hosting_site',
  );

  return $resources;
}

/**
 * Implements hook_hosting_quota_get_usage().
 */
function hosting_site_hosting_quota_get_usage($client, $resource, $start = NULL, $end = NULL) {
  if (hosting_get_client($client) && $resource == 'sites') {
    return db_query("SELECT count(*) FROM {hosting_site} WHERE client = :client AND status <> :status", array(':client' => $client, ':status' => HOSTING_SITE_DELETED))->fetchField();
  }
}

/**
 * Implements hook_hosting_quota_resource_render().
 */
function hosting_site_hosting_quota_resource_render($resource, $usage) {
  return t('@usage sites', array('@usage' => $usage));
}

/**
 * Check for quotas and return an appropriate error message to the site creation form
 */
function hosting_site_quota_exceeded($node) {
  $client = hosting_get_client_by_uname($node->client);
  $client = node_load($client->nid, NULL, TRUE);
  $usage = hosting_site_hosting_quota_get_usage($client->nid, 'sites');
  $limit = $client->quota['sites']['limit'];
  if ($usage >= $limit && !empty($limit)) {
    return t('@client has reached their site quota of @sites. No new sites can be added unless an existing site is deleted or the site quota is increased.', array('@client' => $client->title, '@sites' => $limit));
  }
  return FALSE;
}
