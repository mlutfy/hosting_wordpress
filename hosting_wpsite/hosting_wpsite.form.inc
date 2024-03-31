<?php
/**
 * @file
 * Site node form.
 */

/**
 * Helper function to generate form elements for the wpsite form.
 */
function _hosting_wpsite_field(&$form, $node, $item, $element, $filter_display = 'filter_xss', $editable = FALSE, $show_desc = TRUE) {
  $css_id = str_replace("_", "-", $item);

  $type = $element['#type'];

  if (!isset($node->nid) || $editable) {
    // create it
    if (($element['#type'] == 'radios') && !sizeof($element['#options'])) {
      $form[$item] = array(
        '#type' => 'hidden',
        '#value' => key($element['#options']),
      );
    }
    else {
      $form[$item] = $element;
    }

    if ($show_desc) {
      // the text to display when there are no valid options to select
      $form[$item . '_description'] = array(
        '#prefix' => "<div class='hosting-wpsite-field-description' id='hosting-wpsite-field-{$css_id}-description'>",
        '#suffix' => '</div>',
        '#type' => 'item',
        '#title' => $element['#title'],
        '#description' => isset($element['#description']) ? $element['#description'] : NULL,
        '#markup' => "<div class='placeholder'>" . $filter_display($element['#default_value']) . "</div>",
      );
      if (isset($element['#weight'])) {
        $form[$item . '_description']['#weight'] = $element['#weight'];
      }
    }
  }
  else {
    $type = 'display';

    if ($show_desc) {
      // display it
      $form['info'][$item] = array(
        '#type' => 'item',
        '#title' => $element['#title'],
        '#markup' => $filter_display($element['#default_value']),
        '#required' => FALSE,
      );

      if (isset($element['#weight'])) {
        $form['info'][$item]['#weight'] = $element['#weight'];
      }
    }

    $form[$item] = array(
      '#type' => 'hidden',
      '#value' => $element['#default_value'],
    );
  }

  $form[$item]['#hosting_wpsite_field'] = $item;
  $form[$item]['#hosting_wpsite_field_value'] = $element['#default_value'];
  $form[$item]['#prefix'] = "<div class='hosting-wpsite-field hosting-wpsite-field-{$type}' id='hosting-wpsite-field-{$css_id}'>";
  $form[$item]['#suffix'] = "</div>";
}


/**
 * Pass in a wpsite node and return an array of valid options for it's fields.
 *
 * Modules can define the hook_hosting_wpsite_options_alter function to modify which
 * fields are available for selection.
 *
 * FIXME TODO
 *
 */
function hosting_wpsite_available_options($node, $platform = NULL) {
  // cast to object if it's an array.
  $node = (is_array($node)) ? (object) $node : clone $node;

  $return = array();

  $return['profile'] = array();
  $return['platform'] = array();
  $return['wpsite_language'] = array();

  if (!hosting_feature('client')) {
    // Setting the return value of a text field to null,
    // will signal to the front end that the field needs to
    // be displayed, but is not editable.
    $return['client'] = NULL;
  }

  // Load up the user we'll use to check platform and profile access
  $user = user_load($GLOBALS['user']->uid);

  // Install profiles
  $profiles = hosting_get_profiles();
  $platform_profiles = array();
  foreach ($profiles as $id => $name) {

    // Trim down the list of profiles to those that are available and the user has access to
    // XXX This hack (next 22 lines) hides profiles that can't be accessed
    // Eventually we should lighten up the content of this callback
    $result = db_query("SELECT l.nid
                        FROM {hosting_package_instance} i
                        JOIN {hosting_package} p
                        ON p.nid = i.package_id
                        JOIN {hosting_platform} l
                        ON l.nid = i.rid
                        WHERE i.package_id = :package_id
                        AND p.package_type = :package_type
                        AND l.status = :status;",
                        array(
                          ':package_id' => $id,
                          ':package_type' => 'profile',
                          ':status' => HOSTING_PLATFORM_ENABLED
                        )
                      );
    if (is_null($platform)) {
      $allowed_plats = _hosting_get_allowed_platforms($user->uid);
    }
    else {
      $allowed_plats = array($platform => $platform);
    }
    $access_check = FALSE;

    foreach ($result as $row) {
      if (!is_null($platform) && array_key_exists($row->nid, $allowed_plats)) {
        $platform_profiles[$id] = $name;
      }
      elseif (array_key_exists($row->nid, $allowed_plats)) {
        $access_check = TRUE;
      }
      // open access if no platform access has been set
      // @todo move this into _hosting_get_allowed_platforms
      // @todo confirm this logic
      elseif (!$unrestricted = db_query("SELECT cid
                                         FROM {hosting_platform_client_access}
                                         WHERE pid = :pid LIMIT 1",
                                         array(':pid' => $row['nid'])
                                       )->fetchField()) {
        $access_check = TRUE;
      }
    }
    if (!$access_check) {
      unset($profiles[$id]);
    }
  }
  if (!is_null($platform)) {
    $profiles = $platform_profiles;
  }
  reset($profiles);
  $return['profile'] = array_keys($profiles);

  if (!isset($node->profile)) {
    $node->profile = hosting_get_default_profile($return['profile'][0]);
  }

  // filter the available platforms based on which clients the user has access to.
  if (!is_null($platform)) {
    $node->profile = $return['profile'][0];
    $return['platform'] = array($platform);
  }
  else {
    $options = array();
    $platforms = hosting_get_profile_platforms($node->profile, isset($node->check_profile_migrations) ? $node->check_profile_migrations : FALSE);
    if (sizeof($platforms)) {
      foreach ($platforms as $nid => $title) {
        $platform = node_load($nid);

        if ($platform->platform_status != HOSTING_PLATFORM_LOCKED || user_access('create wpsites on locked platforms')) {
          if (!isset($platform->clients) || sizeof(array_intersect(array_keys($user->client_id), $platform->clients)) || $user->uid == 1) {
            $options[] = $nid;
          }
        }
      }

      $return['platform'] = $options;
    }
  }

  if (!isset($node->platform) || !in_array($node->platform, $return['platform'])) {
    $node->platform = $return['platform'][0];
  }

  $return['wpsite_language'] = array_keys((array) hosting_get_profile_languages($node->profile, $node->platform));

  drupal_alter('hosting_wpsite_options', $return, $node);

  return $return;
}



/**
 * Implements hook_form().
 */
function hosting_wpsite_form($node, &$form_state) {
  $form['#node'] = $node;

  if (isset($node->nid)) {
    $form['info'] = array(
      '#prefix' => '<div class="clear-block" id="hosting-wpsite-edit-info">',
      '#suffix' => '<br /></div>',
      '#weight' => -10,
    );
  }

  _hosting_wpsite_field($form, $node, 'title', array(
    '#type' => 'textfield',
    '#title' => t('Domain name'),
    '#required' => TRUE,
    '#default_value' => isset($node->title) ? strtolower(trim($node->title)) : '',
    '#weight' => -10,
  ));

  $editable = ((!isset($node->client) || !isset($node->nid)) || user_access('administer wpsites')) && hosting_feature('client');
  $add_client_text = '';
  if (user_access('administer clients') || user_access('create client')) {
    $add_client_text = t(' Click !here to add a new client.', array('!here' => l('here', 'node/add/client', array('attributes' => array('target' => '_blank')))));
  }
  _hosting_wpsite_field($form, $node, 'client', array(
    '#type' => 'textfield',
    '#required' => TRUE,
    '#title' => t('Client'),
    '#default_value' => _hosting_client_site_default($node),
    '#description' => t('The client to whom this wpsite belongs.') . $add_client_text,
    '#autocomplete_path' => 'hosting_client/autocomplete/client',
  ), 'filter_xss', $editable);

  // NB: in hosting_site, this calls hosting_get_profile_platforms($selected_profile);
  // but we do not have profiles for wp platforms.
  $platforms = _hosting_get_enabled_wpplatforms();

  _hosting_wpsite_field($form, $node, 'wpplatform', array(
    '#type' => 'radios',
    '#title' => t('Platform'),
    '#required' => TRUE,
    '#description' => t('The platform you want the wpsite to be hosted on.'),
    '#options' => $platforms,
    '#default_value' => isset($node->wpplatform) ? $node->wpplatform : NULL,
    '#ajax' => array(
      'callback' => 'hosting_wpsite_language_callback',
      'wrapper' => 'hosting-wpsite-field-wpsite-language' ,
      'effect' => 'fade',
      'event' => 'change',
      'method' => 'replace',
    ),
  ), '_hosting_node_link');

  // Override the defaults if the profile has been changed.
  if (isset($form_state['values']['wpplatform'])) {
    $selected_platform = $form_state['values']['wpplatform'];
  }
  else{
    $selected_platform = NULL;
  }
  $languages = hosting_get_profile_languages($selected_profile, $selected_platform);
  _hosting_wpsite_field($form, $node, 'wpsite_language', array(
    '#type' => ((count($languages) > 10) ? 'select' : 'radios'),
    '#title' => t('Language'),
    '#description' => t('The language of wpsite being installed.'),
    '#options' => $languages,
    '#required' => TRUE,
    '#default_value' => isset($node->wpsite_language) ? $node->wpsite_language : 'en',
    '#attributes' => array('class' => array("hosting-wpsite-form-wpsite-language-options")),
  ), '_hosting_language_name');

  _hosting_wpsite_field($form, $node, 'db_server', array(
    '#type' => 'radios',
    '#title' => t('Database server'),
    '#required' => TRUE,
    '#description' => t('The database server the wpsite will use to host its content.'),
    '#options' => hosting_get_servers('db'),
    '#default_value' => isset($node->db_server) ? $node->db_server : HOSTING_DEFAULT_DB_SERVER,
  ), '_hosting_node_link');

  foreach (array('verified', 'last_cron', 'wpsite_status') as $extra_attribute) {
    $form["$extra_attribute"] = array(
      '#type' => 'value',
      '#value' => isset($node->$extra_attribute) ? $node->$extra_attribute : NULL,
    );
  }

  // Support for hosting_alias
  if (user_access('create site aliases') && function_exists('hosting_alias_form_data')) {
    $form['#validate'][] = 'hosting_alias_site_form_validate';
    $form = hosting_alias_form_data($form, $form_state);
  }

  return $form;
}

function hosting_wpsite_platform_callback($form, &$form_state){
  return $form['wpplatform'];
}

function hosting_wpsite_language_callback($form, &$form_state){
  return $form['wpsite_language'];
}

/**
 * Implements hook_validate().
 */
function hosting_wpsite_validate($node, &$form) {
  global $user;

  // FIXME TODO
  $valid_options = hosting_wpsite_available_options($node);

  $url = strtolower(trim($node->title)); // domain names are case-insensitive
  if (!_hosting_valid_fqdn($url)) {
    form_set_error('title', t("You have not specified a valid url for this wpsite."));
  }

  $length = strlen($url);
  if ($length > HOSTING_MAX_ALIAS_LENGTH) {
    $long = $length - HOSTING_MAX_ALIAS_LENGTH;
    form_set_error("title", t('The url your provided is @long character(s) too long. Please shorten.', array('@long' => $long)));
  }

  if (isset($node->new_client) && !$node->new_client) {
    $client = hosting_get_client($node->client);
    if (!$node->client || !$client) {
      form_set_error('client', t('Please fill in a valid client'));
    }
    if (!user_access('administer clients') && !array_key_exists($client->nid, hosting_get_client_from_user($user->uid))) {
      form_set_error('client', t('Access denied to client @client', array('@client' => $client->title)));
    }
    $node->client = $client->nid;
  }

  // TODO: maybe we should allow creation of wpsites that conflict with HOSTING_SITE_DISABLED (which would then need to be renamed before being re-enabled)
  if (!hosting_domain_allowed($url, (array) $node)) {
    form_set_error('title', t("The domain name you have specified is already in use."));
  }

  // If the quota module is loaded and this is a new node, check
  // the wpsite quota
  if (!$node->nid && function_exists('hosting_wpsite_quota_exceeded')) {
    $quota_error = hosting_wpsite_quota_exceeded($node);
    if ($quota_error) {
      form_set_error('title', $quota_error);
    }
  }

/* FIXME TODO
  if (!in_array($node->wpplatform, $valid_options['wpplatform']) && !$node->nid) {
    form_set_error('wpplatform', t('Please choose a valid platform'));
  }

  // Check that we are selecting a valid language for this profile, but only when a new wpsite is created.
  if (!in_array($node->wpsite_language, $valid_options['wpsite_language']) && !$node->nid) {
    form_set_error('wpsite_language', t('Please fill in a valid language'));
  }
*/

  // Support for hosting_alias
  // The data gets saved in hosting_wpsite_insert().
  if (function_exists('hosting_alias_validate_alias') && !empty($form_state['values']['aliases'])) {
    $aliases = $form_state['values']['aliases'] = array_filter($form_state['values']['aliases']);
    foreach ($aliases as $key => $alias) {
      hosting_alias_validate_alias($form_state['node'], $alias, $key);
    }
  }
}

/**
 * Implements hook_form_alter().
 *
 * Hide the delete button on wpsite nodes
 */
function hosting_wpsite_form_alter(&$form, &$form_state, $form_id) {
  // Remove delete button from wpsite edit form, unless the wpsite's already been deleted via the Delete task
  if ($form_id == 'wpsite_node_form') {
    $node = $form['#node'];
    if (isset($node->wpsite_status) && $node->wpsite_status !== HOSTING_SITE_DELETED) {
      $form['actions']['delete']['#type'] = 'hidden';
    }
  }
  if (array_key_exists('views_bulk_operations', $form)) {
    // Add our callback, so we can operate on the fully built form.
    $form['#after_build'][] = '_hosting_site_collapse_views_fieldset'; // FIXME wpsite?
  }
}
