<?php

/**
 * A handler for the Platform sites field.
 *
 * @ingroup views_field_handlers
 */
class hosting_platform_handler_field_sites extends views_handler_field {
  function render($values) {
    $value = $values->{$this->field_alias};
    return hosting_site_count($value, array(HOSTING_SITE_ENABLED, HOSTING_SITE_DISABLED, HOSTING_SITE_QUEUED));
  }
}
