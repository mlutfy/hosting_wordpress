<?php
/**
 * @file
 * Expose the wpplatform feature to hostmaster.
 */

/**
 * Implements hook_hosting_feature().
 */
function hosting_wpplatform_hosting_feature() {
  $features['wpplatform'] = array(
    'title' => t('Platform administration'),
    'description' => t('Create and manage wpplatforms.'),
    'status' => HOSTING_FEATURE_REQUIRED,
    'module' => 'hosting_wpplatform',
    'node' => 'wpplatform',
    'group' => 'required',
    'role_permissions' => array(
      'aegir wpplatform manager' => array(
        'administer wpplatforms',
        'create wpplatform',
        'delete wpplatform',
        'edit wpplatform',
        'view locked wpplatforms',
        'view wpplatform',
        'create sites on locked wpplatforms',
      ),
      'aegir client' => array(
        'view wpplatform',
      ),
    ),

  );
  return $features;
}
