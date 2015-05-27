<?php
/**
 * @file
 * Expose the site feature to hostmaster.
 */

/**
 * Implements hook_hosting_feature().
 */
function hosting_wpsite_hosting_feature() {
  $features['wpsite'] = array(
    'title' => t('Wordpress site administration'),
    'description' => t('Create and manage sites.'),
    'status' => HOSTING_FEATURE_DISABLED,
    'module' => 'hosting_wpsite',
    'node' => 'site',
    'group' => 'experimental',
    'role_permissions' => array(
      'aegir platform manager' => array(
        'administer sites',
        'create site',
        'delete site',
        'edit site',
        'view site',
      ),
      'aegir account manager' => array(
        'view site',
      ),
      'aegir client' => array(
        'create site',
        'delete site',
        'view site',
        'edit site',
      ),
    ),

  );
  return $features;
}
