#include <ruby/ruby.h>
#include <ruby/debug.h>
#include <ruby/intern.h>
#include <time.h>
#include <sys/time.h>
#include <pthread.h>

#define BUF_SIZE 500000
static struct {
    long count;
    uint64_t entry_wall_time[BUF_SIZE];
    uint64_t time_diff[BUF_SIZE];
} _wall_times = {
    .count = 0
};

static VALUE rb_mProfileTrace;
static VALUE gc_hook;
static uint64_t _walltime;
struct timeval _walltv;

static uint64_t
get_wall_time()
{
    gettimeofday(&_walltv, NULL);
    _walltime = (uint64_t)_walltv.tv_sec*1e6 + (uint64_t)_walltv.tv_usec;
    return _walltime;
}

static void
profile_trace_entry_hook(rb_event_flag_t event, VALUE data, VALUE self, ID mid, VALUE klass)
{
    VALUE iseq[1];
    int lines[1];

    int num = rb_profile_frames(0, sizeof(iseq) / sizeof(VALUE), iseq, lines);

    VALUE klss = rb_profile_frame_classpath(iseq[0]);
    VALUE meth = rb_profile_frame_method_name(iseq[0]);

    char *ptr = StringValuePtr(klss);
    char *method = StringValuePtr(meth);

    if (RTEST(klss)) {
        _wall_times.entry_wall_time[_wall_times.count] = get_wall_time();
    }
}

static void
profile_trace_exit_hook(rb_event_flag_t event, VALUE data, VALUE self, ID mid, VALUE klass)
{
    VALUE iseq[1];
    int lines[1];

    int num = rb_profile_frames(0, sizeof(iseq) / sizeof(VALUE), iseq, lines);

    VALUE klss = rb_profile_frame_classpath(iseq[0]);
    VALUE meth = rb_profile_frame_method_name(iseq[0]);

    char *ptr = StringValuePtr(klss);
    char *method = StringValuePtr(meth);

    if (RTEST(klss)) {
        _walltime = get_wall_time() - _wall_times.entry_wall_time[_wall_times.count];
        _wall_times.time_diff[_wall_times.count] = _walltime;
        _wall_times.count++;
    }
}

static VALUE
rb_profile_trace_start()
{
    rb_add_event_hook((rb_event_hook_func_t) profile_trace_entry_hook, RUBY_EVENT_CALL, Qnil);
    rb_add_event_hook((rb_event_hook_func_t) profile_trace_exit_hook, RUBY_EVENT_RETURN, Qnil);
    return Qtrue;
}

static VALUE
rb_profile_trace_stop()
{
    rb_remove_event_hook((rb_event_hook_func_t) profile_trace_entry_hook);
    rb_remove_event_hook((rb_event_hook_func_t) profile_trace_exit_hook);
    return Qtrue;
}

static VALUE
rb_profile_trace_count()
{
    int total_time = 0;
    uint64_t total_time_diff = 0;

    for(long i = 0; i < _wall_times.count; i++) {
        total_time_diff = total_time_diff + _wall_times.time_diff[i];
    }

    total_time = NUM2INT(ULL2NUM(total_time_diff));
    printf("A#to_string called %lu times\n", _wall_times.count);
    printf("Aggregate time spent by A#to_string: %llu micro seconds", total_time_diff);
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

