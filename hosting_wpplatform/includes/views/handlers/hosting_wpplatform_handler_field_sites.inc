<?php

/**
 * A handler for the WP Platform sites field.
 *
 * @ingroup views_field_handlers
 */
class hosting_wpplatform_handler_field_sites extends views_handler_field {
  function render($values) {
    $value = $values->{$this->field_alias};
    return hosting_wpsite_count($value, array(HOSTING_SITE_ENABLED, HOSTING_SITE_DISABLED, HOSTING_SITE_QUEUED));
  }
}
