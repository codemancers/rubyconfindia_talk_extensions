#include <ruby/ruby.h>
#include <ruby/debug.h>
#include <ruby/intern.h>
#include <pthread.h>

static struct {
    long count;
}
_counter = {
    .count = 0
};

static VALUE rb_mProfileTrace;
static VALUE gc_hook;

static void
profile_trace_hook(rb_event_flag_t event, VALUE data, VALUE self, ID mid, VALUE klass)
{
    VALUE klass_called = rb_intern(rb_class2name(klass));
    VALUE fixnum_value = rb_intern("Fixnum");

    VALUE method_id = rb_intern("to_s");

    if (method_id == mid && fixnum_value == klass_called) {
        _counter.count++;
    }
}

static VALUE
rb_profile_trace_start()
{
    rb_add_event_hook((rb_event_hook_func_t) profile_trace_hook, RUBY_EVENT_CALL | RUBY_EVENT_C_CALL, Qnil);
    return Qtrue;
}

static VALUE
rb_profile_trace_stop()
{
    rb_remove_event_hook((rb_event_hook_func_t) profile_trace_hook);
    return Qtrue;
}

static VALUE
rb_profile_trace_count()
{
    printf("Fixnum#to_s was called %lu times\n", _counter.count);
    return Qnil;
}

static void
profile_frames_example_gc_mark()
{
    rb_gc_mark_maybe(Qfalse);
}

void
Init_rb_profile_trace(void)
{
    gc_hook = Data_Wrap_Struct(rb_cObject, profile_frames_example_gc_mark, NULL, NULL);
    rb_global_variable(&gc_hook);

    rb_mProfileTrace = rb_define_module("ProfileTrace");
    rb_define_singleton_method(rb_mProfileTrace, "start", rb_profile_trace_start, 0);
    rb_define_singleton_method(rb_mProfileTrace, "stop", rb_profile_trace_stop, 0);
    rb_define_singleton_method(rb_mProfileTrace, "print", rb_profile_trace_count, 0);
}

