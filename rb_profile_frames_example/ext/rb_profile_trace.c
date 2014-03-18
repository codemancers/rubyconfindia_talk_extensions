#include <ruby/ruby.h>
#include <ruby/debug.h>
#include <ruby/st.h>
#include <ruby/io.h>
#include <ruby/intern.h>
#include <signal.h>
#include <sys/time.h>
#include <pthread.h>


static VALUE rb_mProfileTrace;
static VALUE gc_hook;

static void
profile_trace_hook(rb_event_flag_t event, VALUE data, VALUE self, ID mid, VALUE klss)
{
    VALUE iseq[2];
    int lines[2];

    int num = rb_profile_frames(0, sizeof(iseq) / sizeof(VALUE), iseq, lines);

    VALUE klass         = rb_profile_frame_classpath(iseq[0]);
    VALUE meth          = rb_profile_frame_method_name(iseq[0]);
    VALUE labl          = rb_profile_frame_base_label(iseq[0]);
    VALUE qmeth         = rb_profile_frame_qualified_method_name(iseq[0]);
    VALUE filepath      = rb_profile_frame_absolute_path(iseq[0]);
    VALUE lineno        = rb_profile_frame_first_lineno(iseq[0]);

    char *klass_name    = StringValuePtr(klass);
    char *method_name   = StringValuePtr(meth);
    char *label         = StringValuePtr(labl);
    char *q_method_name = StringValuePtr(qmeth);
    char *file_path     = StringValuePtr(filepath);
    int linno           = NUM2INT(lineno);

    printf("The class name is: %s\n", klass_name);
    printf("The method name is: %s\n", method_name);
    printf("The fully qualified method name is: %s\n", q_method_name);
    printf("The label is: %s\n", label);
    printf("The file path where the method was called: %s\n", file_path);
    printf("The line where the method is defined: %d\n", linno);
    printf("The line where the method is defined: %d\n", lines[0]);
}

static void
profile_frames_example_gc_mark()
{
    rb_gc_mark_maybe(Qfalse);
}

static VALUE
rb_profile_trace_start()
{
    rb_add_event_hook((rb_event_hook_func_t) profile_trace_hook, RUBY_EVENT_CALL, Qnil);
    return Qtrue;
}

static VALUE
rb_profile_trace_stop()
{
    rb_remove_event_hook((rb_event_hook_func_t) profile_trace_hook);
    return Qtrue;
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

