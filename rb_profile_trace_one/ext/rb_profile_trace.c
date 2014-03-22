#include <ruby/ruby.h>
#include <ruby/debug.h>
#include <ruby/intern.h>
#include <pthread.h>

static VALUE rb_mProfileTrace;
static VALUE gc_hook;

static void
profile_trace_hook(rb_event_flag_t event, VALUE data, VALUE self, ID mid, VALUE klass)
{
    VALUE iseq[1];
    int lines[1];

    int num = rb_profile_frames(0, sizeof(iseq) / sizeof(VALUE), iseq, lines);

    VALUE klss = rb_profile_frame_classpath(iseq[0]);

    char *ptr = StringValuePtr(klss);
    printf("%s\n", ptr);
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
}

