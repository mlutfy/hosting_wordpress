<?php
/**
 * @file
 * Template file for a drushrc wpsite file.
 */
print "<?php \n"; ?>

<?php foreach ($option_keys as $key) {
  print "\n\$options['$key'] = ". var_export(${$key}, TRUE) .';';
}
?>

# Aegir additions
<?php foreach (array('db_type', 'db_port', 'db_host', 'db_user', 'db_passwd', 'db_name', 'wp_content_dir', 'wp_content_url') as $key) { ?>
$_SERVER['<?php print $key; ?>'] = $options['<?php print $key; ?>'];
<?php } ?>
# WordPress salts
<?php foreach (['AUTH_KEY', 'SECURE_AUTH_KEY', 'LOGGED_IN_KEY', 'NONCE_KEY', 'AUTH_SALT', 'SECURE_AUTH_SALT', 'LOGGED_IN_SALT', 'NONCE_SALT'] as $key) { ?>
if (!defined('<?php print $key; ?>')) {
  define('<?php print $key; ?>', $options['salt_<?php print $key; ?>']);
}
<?php } ?>
# local non-aegir-generated additions
@include_once('<?php print $this->site_path  ?>/local.drushrc.php');
