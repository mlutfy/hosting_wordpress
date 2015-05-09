(function($) {
  Drupal.behaviors.aegir_make_working_copy = function (context) {
    $('.hosting-wpplatform-working-copy-source:not(.hosting-wpplatform-working-copy-processed)', context)
      .addClass('hosting-wpplatform-working-copy-processed')
      .bind('change', function() {
        Drupal.hosting_wpplatform_working_copy.update_visibility($(this));
      })
      .bind('keyup', function() {
        Drupal.hosting_wpplatform_working_copy.update_visibility($(this));
      })
      .each(function() {
        Drupal.hosting_wpplatform_working_copy.update_visibility($(this));
      });
  }
  
  Drupal.hosting_wpplatform_working_copy = Drupal.hosting_wpplatform_working_copy || {};
  
  Drupal.hosting_wpplatform_working_copy.update_visibility = function($elem) {
    if ($elem.val()) {
      $('.hosting-wpplatform-working-copy-target').parents('.form-item')
        .show();
    }
    else {
      $('.hosting-wpplatform-working-copy-target').parents('.form-item')
        .hide();
    }
  }
  
})(jQuery);
