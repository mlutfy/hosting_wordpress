<?php

/**
 * @file
 * Implementation for provision-wordpress-backup
 * A lot was copied from platform/backup.provision.inc
 */

const PROVISION_BACKUP_WORDPRESS_EXCLUDED_DIRECTORIES = [
  './wp-content/uploads/civicrm/templates_c',
];

/**
 * Make sure the site is installed and enabled, and that we have a valid target to back up to.
 */
function drush_provision_wordpress_backup_validate($backup_file = NULL) {
  if (!is_dir(d()->wpplatform->server->backup_path)) {
    drush_set_error('PROVISION_BACKUP_PATH_NOT_FOUND');
  }

  if ($backup_file) {
    if (provision_file()->exists($backup_file)->status()) {
      drush_set_error('PROVISION_BACKUP_ALREADY_EXISTS', dt('Back up file @path already exists.', array('@path' => $backup_file)));
    }
    else {
      drush_log(dt('Backing site up to @path.', array('@path' => $backup_file)));
      drush_set_option('backup_file', $backup_file);
    }
  }

  if (!$backup_file) {
    $suggested = drush_provision_wordpress_backup_suggest_filename();
    drush_set_option('backup_file', $suggested);
  }
}

/**
 * Implentation of hook_provision_wordpress_backup()
 */
function drush_provision_wordpress_backup() {
  $backup_file = drush_get_option('backup_file');

  drush_log(dt("WordPress: generating SQL dump"), 'ok');
  d()->service('db')->generate_dump();
  drush_log(dt("WordPress: SQL dump generated"), 'ok');

  // Adds the site directory into the backup file
  drush_log(dt("Adding !site directory to !backup_file", array('!site' => d()->site_path, '!backup_file' => $backup_file)), 'backup');

  $olddir = getcwd();

  if (!chdir(d()->site_path)) {
    return drush_set_error('PROVISION_BACKUP_PATH_NOT_FOUND', dt('cannot change directory to %dir', array('%dir' => d()->site_path)));
  }

  $command_base = "tar cpf";
  $command_arguments = " %s" . drush_provision_backup_wordpress_get_exclusions() . " .";

  if (substr($backup_file, -2) == 'gz') {
    $command = $command_base . 'z' . $command_arguments;
  }
  elseif (substr($backup_file, -3) == 'bz2') {
    $command = $command_base . 'j' . $command_arguments;
  } else {
    $command = $command_base . $command_arguments;
  }

  drush_log(dt("WordPress: generating backup archive.."), 'ok');
  $result = drush_shell_exec($command,  $backup_file);

  // Get the size of the backup
  $size = filesize($backup_file);
  drush_set_option('backup_file_size', $size);
  drush_log(dt("WordPress: backup archive generated (!file, !size kB)", ['!file' => $backup_file, '!size' => $size]), 'ok');

  provision_file()->unlink(d()->site_path . '/database.sql');
  drush_log(dt("WordPress: deleted the SQL dump"), 'ok');

  chdir($olddir);

  if (!$result && !drush_get_option('force', false)) {
    drush_set_error('PROVISION_BACKUP_FAILED', dt("Could not back up sites directory for WordPress"));
  }
}

function drush_provision_backup_wordpress_get_exclusions() {
  $directories = PROVISION_BACKUP_WORDPRESS_EXCLUDED_DIRECTORIES;
  drush_command_invoke_all_ref('provision_backup_wordpress_exclusions_alter', $directories);

  $exclusions = "";
  foreach ($directories as $directory) {
    $exclusions .= " --exclude='$directory'";
  }
  return $exclusions;
}

/**
 * Util function to suggest a filename for a new site backup.
 */
function drush_provision_wordpress_backup_suggest_filename() {
  $suffix = drush_get_option('provision_backup_suffix', '.tar.gz');
  $suggested = d()->platform->server->backup_path . '/' . d()->uri . '-' . date("Ymd.His", time()) . $suffix;

  // Use format of mysite.com-2008-01-02, if already existing, add number.
  $count = 0;
  while (is_file($suggested)) {
    $count++;
    $suggested = d()->platform->server->backup_path . '/' . d()->uri . '-' .  date('Ymd.His', time()) . '_' . $count . $suffix;
  }
  return $suggested;
}
