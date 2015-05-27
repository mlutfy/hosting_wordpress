<?php
/**
 * @file
 * Expose the wpsite feature to hostmaster.
 */

/**
 * Implements hook_hosting_feature().
 */
function hosting_wpsite_hosting_feature() {
  $features['wpsite'] = array(
    'title' => t('Wordpress site administration'),
    'description' => t('Create and manage WordPress sites.'),
    'status' => HOSTING_FEATURE_DISABLED,
    'module' => 'hosting_wpsite',
    'node' => 'wpsite',
    'group' => 'experimental',
    'role_permissions' => array(
      'aegir platform manager' => array(
        'administer wpsites',
        'create wpsite',
        'delete wpsite',
        'edit wpsite',
        'view wpsite',
      ),
      'aegir account manager' => array(
        'view wpsite',
      ),
      'aegir client' => array(
        'create wpsite',
        'delete wpsite',
        'view wpsite',
        'edit wpsite',
      ),
    ),
  );
  return $features;
}
