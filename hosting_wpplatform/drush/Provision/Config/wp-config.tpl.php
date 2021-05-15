<?php

/**
 * @file
 * Template file for a wp-config.php file.
 *
 * This is based on wp-config-sample.php, but we use the $_SERVER
 * variables which are passed onto the vhost.
 */
print "<?php \n";
?>

/**
 * The base configurations of the WordPress.
 *
 * This file has the following configurations: MySQL settings, Table Prefix,
 * Secret Keys, and ABSPATH. You can find more information by visiting
 * {@link https://codex.wordpress.org/Editing_wp-config.php Editing wp-config.php}
 * Codex page. You can get the MySQL settings from your web host.
 *
 * This file is used by the wp-config.php creation script during the
 * installation. You don't have to use the web site, you can just copy this file
 * to "wp-config.php" and fill in the values.
 *
 * @package WordPress
 */

/**
 * Aegir-specific - Adds support for using civix and cv from the CLI.
 *
 * It will traverse the directory hierarchy up from the current working directory
 * looking for a drushrc.php file. It also looks for a wp-config.php file, to avoid
 * going higher than the site root.
 */
if (php_sapi_name() == "cli") {
  $directory = getcwd();

  while (dirname($directory) != '/') {
    if (file_exists($directory . '/drushrc.php')) {
      require_once($directory . '/drushrc.php');
      break;
    }
    if (file_exists($directory . '/wp-config.php')) {
      break;
    }

    $directory = dirname(dirname($directory . '/../'));
  }
}

// ** MySQL settings - You can get this info from your web host ** //
/** The name of the database for WordPress */
define('DB_NAME', $_SERVER['db_name']);

/** MySQL database username */
define('DB_USER', $_SERVER['db_user']);

/** MySQL database password */
define('DB_PASSWORD', $_SERVER['db_passwd']);

/** MySQL hostname */
define('DB_HOST', $_SERVER['db_host']);

/** Content directory */
define('WP_CONTENT_DIR', $_SERVER['wp_content_dir']);
define('WP_CONTENT_URL', $_SERVER['wp_content_url']);

/** Database Charset to use in creating database tables. */
define('DB_CHARSET', 'utf8');

/** The Database Collate type. Don't change this if in doubt. */
define('DB_COLLATE', '');

/** Required by CiviCRM extensions */
define('CIVICRM_CMSDIR', '<?php print $this->root; ?>');

/** Load Aegir global settings */
if (file_exists('/var/aegir/config/includes/global.inc')) {
  require_once '/var/aegir/config/includes/global.inc';
}

/**
 * WordPress Database Table prefix.
 *
 * You can have multiple installations in one database if you give each a unique
 * prefix. Only numbers, letters, and underscores please!
 *
 * NB: this can be overridden by the site-specific wp-config.php.
 */
$table_prefix  = 'wp_';

/**#@+
 * Authentication Unique Keys and Salts.
 *
 * Change these to different unique phrases!
 * You can generate these using the {@link https://api.wordpress.org/secret-key/1.1/salt/ WordPress.org secret-key service}
 * You can change these at any point in time to invalidate all existing cookies. This will force all users to have to log in again.
 *
 * @since 2.6.0
 */
if (file_exists($_SERVER['wp_content_dir'] . '/../wp-config.php')) {
  require_once $_SERVER['wp_content_dir'] . '/../wp-config.php';
}

if (!defined('AUTH_KEY')) {
  define('AUTH_KEY',         'put your unique phrase here');
  define('SECURE_AUTH_KEY',  'put your unique phrase here');
  define('LOGGED_IN_KEY',    'put your unique phrase here');
  define('NONCE_KEY',        'put your unique phrase here');
  define('AUTH_SALT',        'put your unique phrase here');
  define('SECURE_AUTH_SALT', 'put your unique phrase here');
  define('LOGGED_IN_SALT',   'put your unique phrase here');
  define('NONCE_SALT',       'put your unique phrase here');
}

/**#@-*/

/**
 * For developers: WordPress debugging mode.
 *
 * Change this to true to enable the display of notices during development.
 * It is strongly recommended that plugin and theme developers use WP_DEBUG
 * in their development environments.
 */
if (!defined('WP_DEBUG')) {
  define('WP_DEBUG', false);
}

/* That's all, stop editing! Happy blogging. */

/** Absolute path to the WordPress directory. */

// If run from Aegir (platform verify), stop here.
if ( !defined('ABSPATH') )
	define('ABSPATH', dirname(__FILE__) . '/');

/** Sets up WordPress vars and included files. */
require_once(ABSPATH . 'wp-settings.php');
