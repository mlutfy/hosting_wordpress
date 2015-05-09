<?php

/**
 * A handler for the Platform status field.
 *
 * @ingroup views_field_handlers
 */
class hosting_platform_handler_field_status extends hosting_site_handler_field_status {
  function render($values) {
    $value = $values->{$this->field_alias};
    $output = _hosting_platform_status($value);

    switch ($this->options['status_mode']) {
      case 'image':
        return "<span class='hosting-status hosting-status-icon'></span>";

      case 'text_image':
        return "<span class='hosting-status'>{$output}</span>";

      case 'class':
        return _hosting_platform_list_class($value);
    }
    return $output;
  }
}
