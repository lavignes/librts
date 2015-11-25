#ifndef CHLORINE2
#define CHLORINE2

#include <sys/time.h>

#include <pthread.h>

#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdarg.h>
#include <string.h>

#ifndef CHLORINE_NO_COLORS
#define __CL_RESET "\x1B[0m"
#define __CL_BOLD "\x1B[1m"
#define __CL_UL "\x1B[4m"
#define __CL_TAB "        "

#define __CL_BLACK "0"
#define __CL_RED "1"
#define __CL_GREEN "2"
#define __CL_YELLOW "3"
#define __CL_BLUE "4"
#define __CL_MAGENTA "5"
#define __CL_CYAN "6"
#define __CL_WHITE "7"
#define __CL_FG(color) "\x1B[3"color"m"
#define __CL_BG(color) "\x1B[4"color"m"
#else
#define __CL_RESET ""
  #define __CL_BOLD ""
  #define __CL_UL ""
  #define __CL_TAB "        "

  #define __CL_BLACK ""
  #define __CL_RED ""
  #define __CL_GREEN ""
  #define __CL_YELLOW ""
  #define __CL_BLUE ""
  #define __CL_MAGENTA ""
  #define __CL_CYAN ""
  #define __CL_WHITE ""
  #define __CL_FG(color) ""
  #define __CL_BG(color) ""
#endif

pthread_key_t __CLEnvKey;

typedef struct __CLSpecEnv {
    void* userdata;
    bool serial_pass;
    bool failed;
    char* test_name;
    char* output_buffer;
    size_t num_failed_asserts;
    size_t num_passed_asserts;
} __CLSpecEnv;

typedef void* (*__CLSpecType)(__CLSpecEnv *);

typedef struct __CLWork {
    __CLSpecType* specs;
    __CLSpecEnv* envs;
    size_t num_specs;
    size_t index;
    bool serial_pass;
    pthread_mutex_t lock;
} __CLWork;

/**
 * Options used when building a CL_SPEC.
 */
typedef enum CLOptions {
    CL_OPTION_NONE          = 0,

    /**
     * The SPEC will skip its setup fixture, even it doesn't
     * use the default setup fixture (CL_SETUP).
     */
            CL_OPTION_SKIP_SETUP    = 1 << 0,

    /**
     * The SPEC will skip its teardown fixture, even it doesn't
     * use the default teardown fixture (CL_TEARDOWN).
     */
            CL_OPTION_SKIP_TEARDOWN = 1 << 1,

    /**
     * The SPEC will skip setup and teardown.
     */
            CL_OPTION_SKIP_SETUP_AND_TEARDOWN = CL_OPTION_SKIP_SETUP | CL_OPTION_SKIP_TEARDOWN,

    /**
     * The SPEC will run in serial, after all parallel SPECS have ran, regardless
     * of the number of parallel jobs in the bundle.
     */
            CL_OPTION_SERIAL        = 1 << 2,
} CLOptions;

typedef void (*__CLSetupType)();

__CLSetupType __cl_setup;
__CLSetupType __cl_teardown;
__CLSetupType __cl_setup_once;
__CLSetupType __cl_teardown_once;

void __cl_env_init(__CLSpecEnv* env) {
    env->userdata = NULL;
    env->serial_pass = false;
    env->failed = false;
    env->test_name = NULL;
    env->output_buffer = malloc(16);
    env->output_buffer[0] = '\0';
    env->num_failed_asserts = 0;
    env->num_passed_asserts = 0;
}

__CLSpecEnv* __cl_env() {
    return pthread_getspecific(__CLEnvKey);
}

void __cl_vappend(const char* format, va_list args) {
    __CLSpecEnv *env = __cl_env();
    size_t cursor = 0;
    if (env->output_buffer) {
        cursor = strlen(env->output_buffer);
    }
    char* new_output = NULL;
    size_t new_output_size = vasprintf(&new_output, format, args) + 1;
    env->output_buffer = realloc(env->output_buffer, cursor + new_output_size);
    strncpy(env->output_buffer + cursor, new_output, new_output_size);
    free(new_output);
}

void __cl_append(const char* format, ...) {
    va_list args;
    va_start(args, format);
    __cl_vappend(format, args);
    va_end(args);
}

void __cl_print(const char* format, ...) {
    va_list args;
    va_start (args, format);
    vfprintf(stderr, format, args);
    va_end(args);
}

/**
 * Return the SPEC-specific custom pointer set using cl_set_userdata().
 */
void* cl_get_userdata() {
    __CLSpecEnv* env = __cl_env();
    if (!env) {
        __cl_print(__CL_BOLD __CL_FG(__CL_YELLOW) "[ERROR] " __CL_RESET
                           "Detecting call to cl_get_userdata() from outside the scope "
                           "of a SPEC\n\n");
        return NULL;
    }
    return env->userdata;
}

/**
 * Set the SPEC-specific custom pointer. Get it with cl_get_userdata().
 */
void cl_set_userdata(void* userdata) {
    __CLSpecEnv* env = __cl_env();
    if (!env) {
        __cl_print(__CL_BOLD __CL_FG(__CL_YELLOW) "[ERROR] " __CL_RESET
                           "Detecting call to cl_set_userdata() from outside the scope "
                           "of a SPEC\n\n");
        return;
    }
    env->userdata = userdata;
}

/**
 * Return the number of passed assertions so far in the SPEC.
 */
size_t cl_num_passed() {
    __CLSpecEnv* env = __cl_env();
    if (!env) {
        __cl_print(__CL_BOLD __CL_FG(__CL_YELLOW) "[ERROR] " __CL_RESET
                           "Detecting call to cl_num_passed() from outside the scope "
                           "of a SPEC\n\n");
        return 0;
    }
    return env->num_passed_asserts;
}

/**
 * Return the number of failed assertions so far in the SPEC.
 */
size_t cl_num_failed() {
    __CLSpecEnv* env = __cl_env();
    if (!env) {
        __cl_print(__CL_BOLD __CL_FG(__CL_YELLOW) "[ERROR] " __CL_RESET
                           "Detecting call to cl_num_passed() from outside the scope "
                           "of a SPEC\n\n");
        return 0;
    }
    return env->num_failed_asserts;
}

/**
 * Return the name of the SPEC as a string.
 */
const char* cl_get_name() {
    __CLSpecEnv* env = __cl_env();
    if (!env) {
        __cl_print(__CL_BOLD __CL_FG(__CL_YELLOW) "[ERROR] " __CL_RESET
                           "Detecting call to cl_get_name() from outside the scope "
                           "of a SPEC\n\n");
        return NULL;
    }
    return env->test_name;
}

/**
 * Return whether the SPEC has failed.
 */
bool cl_has_failed() {
    __CLSpecEnv* env = __cl_env();
    if (!env) {
        __cl_print(__CL_BOLD __CL_FG(__CL_YELLOW) "[ERROR] " __CL_RESET
                           "Detecting call to cl_has_failed() from outside the scope "
                           "of a SPEC\n\n");
        return false;
    }
    return env->failed;
}

void __cl_info(const char* format, ...) {
    __cl_append(__CL_BOLD __CL_FG(__CL_CYAN) "[INFO]" __CL_RESET " ");
    va_list args;
    va_start (args, format);
    __cl_vappend(format, args);
    va_end(args);
}

void __cl_error(const char* format, ...) {
    __cl_append(__CL_BOLD __CL_FG(__CL_YELLOW) __CL_TAB "[ERROR]" __CL_RESET " ");
    va_list args;
    va_start (args, format);
    __cl_vappend(format, args);
    va_end(args);
}

void __cl_pass(const char* format, ...) {
    __cl_append(__CL_BOLD __CL_FG(__CL_GREEN) __CL_TAB "[PASS] " __CL_RESET " ");
    va_list args;
    va_start (args, format);
    __cl_vappend(format, args);
    va_end(args);
}

void __cl_fail(const char* format, ...) {
    __cl_append(__CL_BOLD __CL_FG(__CL_RED) __CL_TAB "[FAIL] " __CL_RESET " ");
    va_list args;
    va_start (args, format);
    __cl_vappend(format, args);
    va_end(args);
}

void __cl_log(const char* format, ...) {
    __cl_append(__CL_BOLD __CL_FG(__CL_RED) __CL_TAB "[LOG]  " __CL_RESET " ");
    va_list args;
    va_start (args, format);
    __cl_vappend(format, args);
    va_end(args);
    __cl_append("\n");
}

double __cl_time() {
    struct timeval t;
    gettimeofday(&t, NULL);
    return (double) t.tv_sec + t.tv_usec / 1000000.0;
}

/**
 * Write a message to the SPEC's log.
 */
#define cl_log(format, ...)                                                  \
do {                                                                         \
  if (!__cl_env()) {                                                         \
        __cl_print(__CL_BOLD __CL_FG(__CL_YELLOW) "[ERROR] " __CL_RESET      \
               "Detecting call to cl_log() from outside the scope "          \
               "of a SPEC at %s:%d\n\n", __FILE__, __LINE__);                \
    return;                                                                  \
  }                                                                          \
  __cl_log(format, ##__VA_ARGS__);                                           \
} while (0)                                                                  \

/**
 * Force a SPEC to fail and exit.
 */
#define cl_abort()                                                           \
do {                                                                         \
  __CLSpecEnv* env = __cl_env();                                             \
  if (!env) {                                                                \
    __cl_print(__CL_BOLD __CL_FG(__CL_YELLOW) "[ERROR] " __CL_RESET          \
               "Detecting call to cl_abort() from outside the scope "        \
               "of a SPEC at %s:%d\n\n", __FILE__, __LINE__);                \
  }                                                                          \
  env->failed = true;                                                        \
  __cl_fail("Aborted\n\n");                                                  \
  __cl_append("======================================================\n\n"); \
  pthread_exit(NULL);                                                        \
} while(0)                                                                   \

/**
 * Return false if the running SPEC was created using CL_OPTION_SERIAL.
 */
bool cl_is_parallel() {
    __CLSpecEnv* env = __cl_env();
    if (!env) {
        __cl_print(__CL_BOLD __CL_FG(__CL_YELLOW) "[ERROR] " __CL_RESET
                           "Detecting call to cl_is_parallel() from outside the scope "
                           "of a SPEC\n\n");
        return false;
    }
    return !env->serial_pass;
}

/**
 * Assert an expression is true, and print a message if the test fails.
 */
#define cl_assert_msg(test, format, ...)                                     \
do {                                                                         \
  __CLSpecEnv* env = __cl_env();                                             \
  if (!env) {                                                                \
    __cl_print(__CL_BOLD __CL_FG(__CL_YELLOW) "[ERROR] " __CL_RESET          \
               "Detecting call to cl_assert() from outside the scope "       \
               "of a SPEC at %s:%d\n\n", __FILE__, __LINE__);                \
  } else if (!(test)) {                                                      \
    env->num_failed_asserts++;                                               \
    /* Log the assert failure */                                             \
    __cl_error("Assertion Failed: " __CL_FG(__CL_RED) __CL_BOLD              \
               "%s\n" __CL_RESET __CL_TAB __CL_TAB "in %s:%d\n"              \
               __CL_TAB __CL_TAB, #test, __FILE__, __LINE__);                \
    __cl_append(__CL_FG(__CL_BLUE));                                         \
    __cl_append(format, ##__VA_ARGS__);                                      \
    __cl_append(__CL_RESET "\n\n");                                          \
  } else {                                                                   \
    env->num_passed_asserts++;                                               \
  }                                                                          \
} while(0)                                                                   \

/**
 * Assert an expression is true.
 */
#define cl_assert(test) cl_assert_msg(test, "")

/**
 * The default fixture that is run repeatedly before every test.
 */
#define CL_SETUP                                                             \
void __cl_setup_impl();                                                      \
__CLSetupType __cl_setup = __cl_setup_impl;                                  \
void __cl_setup_impl()                                                       \

/**
 * The default fixture that is run repeatedly after every test.
 */
#define CL_TEARDOWN                                                          \
void __cl_teardown_impl();                                                   \
__CLSetupType __cl_teardown = __cl_teardown_impl;                            \
void __cl_teardown_impl()                                                    \

/**
 * A fixture ran once before all tests. You should not call any
 * cl_* functions from this fixture. If you do, an error will be added
 * to the log.
 */
#define CL_SETUP_ONCE                                                        \
void __cl_setup_once_impl();                                                 \
__CLSetupType __cl_setup_once = __cl_setup_once_impl;                        \
void __cl_setup_once_impl()                                                  \

/**
 * A fixture ran once after all tests. You should not call any
 * cl_* functions from this fixture. If you do, an error will be added
 * to the log.
 */
#define CL_TEARDOWN_ONCE                                                     \
void __cl_teardown_once_impl();                                              \
__CLSetupType __cl_teardown_once = __cl_teardown_once_impl;                  \
void __cl_teardown_once_impl()                                               \

void* __cl_spec(
        const char* name,
        __CLSetupType spec,
        __CLSpecEnv* env,
        __CLSetupType setup,
        __CLSetupType teardown,
        CLOptions options)
{
    if (options & CL_OPTION_SERIAL) {
        if (!env->serial_pass) {
            return NULL;
        }
    } else if (env->serial_pass) {
        return NULL;
    }
    env->test_name = (char *) name;
    pthread_setspecific(__CLEnvKey, env);
    __cl_info("Executing SPEC => " __CL_FG(__CL_CYAN) __CL_BOLD
                      "%s\n\n" __CL_RESET, name);
    double time_start = __cl_time();
    if (!(options & CL_OPTION_SKIP_SETUP) && __cl_setup) {
        __cl_setup();
    }
    if (setup) {
        setup();
    }
    spec();
    if (teardown) {
        teardown();
    }
    if (!(options & CL_OPTION_SKIP_TEARDOWN) && __cl_teardown) {
        __cl_teardown();
    }
    double elapsed = __cl_time() - time_start;
    size_t passed = env->num_passed_asserts;
    size_t failed = env->num_failed_asserts;
    size_t num_asserts = passed + failed;
    if (failed == 0) {
        __cl_pass("Passed SPEC in %.4f s -> " __CL_FG(__CL_GREEN) "%zu/%zu fail"
                          __CL_RESET ", " __CL_FG(__CL_GREEN) "%zu/%zu pass" __CL_RESET "\n\n",
                  elapsed, failed, num_asserts, passed, num_asserts);
    } else {
        env->failed = true;
        __cl_fail("Failed SPEC in %.4f s -> " __CL_FG(__CL_RED) "%zu/%zu fail"
                          __CL_RESET ", " __CL_FG(__CL_RED) "%zu/%zu pass" __CL_RESET "\n\n",
                  elapsed, failed, num_asserts, passed, num_asserts);
    }
    __cl_append("======================================================\n\n");
    return NULL;
}

__CLSetupType __cl_fixture_CL_FIXTURE_NONE = NULL;

/**
 * Define a custom fixture.
 */
#define CL_FIXTURE(name) void __cl_fixture_##name ()

/**
 * Call a fixture from any SPEC.
 */
#define CL_CALL_FIXTURE(name)                                                \
__cl_fixture_##name()                                                        \

/**
 * Define a SPEC with custom fixtures and options.
 */
#define CL_SPEC_FIXTURE_OPTIONS(name, setup, teardown, options)              \
void __cl_spec_##name ();                                                    \
void* name (__CLSpecEnv* env) {                                              \
  return __cl_spec(#name, __cl_spec_##name, env,                             \
                   __cl_fixture_##setup,                                     \
                   __cl_fixture_##teardown, options);                        \
}                                                                            \
void __cl_spec_##name ()                                                     \

/**
 * Define a SPEC with options.
 */
#define CL_SPEC_OPTIONS(name, options)                                       \
CL_SPEC_FIXTURE_OPTIONS(name, CL_FIXTURE_NONE, CL_FIXTURE_NONE, options)     \

/**
 * Define a SPEC with custom fixtures.
 */
#define CL_SPEC_FIXTURE(name, setup, teardown)                               \
CL_SPEC_FIXTURE_OPTIONS(name, setup, teardown, CL_OPTION_NONE)               \

/**
 * Define a SPEC.
 */
#define CL_SPEC(name) CL_SPEC_OPTIONS(name, CL_OPTION_NONE)

void *__cl_do_work(void *ptr)
{
    __CLWork *work = ptr;
    size_t index = 0;
    while (true) {
        pthread_mutex_lock(&work->lock);
        index = work->index;
        if (index == work->num_specs) {
            pthread_mutex_unlock(&work->lock);
            break;
        }
        work->index++;
        pthread_mutex_unlock(&work->lock);
        __CLSpecEnv *env = &work->envs[index];
        if (!work->serial_pass) {
            __cl_env_init(env);
        }
        env->serial_pass = work->serial_pass;
        work->specs[index](env);
    }
    return NULL;
}

int __cl_main_parallel(
        int argc,
        char** argv,
        size_t num_threads,
        __CLSpecType* specs,
        size_t num_specs,
        const char* filename)
{
    pthread_key_create(&__CLEnvKey, NULL);
    size_t i;
    size_t failed = 0;
    double time_start = __cl_time();
    __cl_print(__CL_BOLD __CL_FG(__CL_CYAN) "[INFO] " __CL_RESET
                       "Running BUNDLE: %s\n", filename);
    __cl_print(__CL_BOLD __CL_FG(__CL_CYAN) "[INFO] " __CL_RESET
                       "Using %zu threads\n\n", num_threads);
    if (__cl_setup_once) {
        __cl_setup_once();
    }
    pthread_t* threads = malloc(sizeof(pthread_t) * num_threads);
    __CLWork work = {
            .specs = specs,
            .envs = malloc(sizeof(__CLSpecEnv) * num_specs),
            .num_specs = num_specs,
            .index = 0,
            .serial_pass = false,
    };
    pthread_mutex_init(&work.lock, NULL);
    for (i = 0; i < num_threads; i++) {
        pthread_create(&threads[i], NULL, __cl_do_work, &work);
    }
    for (i = 0; i < num_threads; i++) {
        pthread_join(threads[i], NULL);
    }
    free(threads);
    work.index = 0;
    work.serial_pass = true;
    __cl_do_work(&work);
    pthread_mutex_destroy(&work.lock);
    for (i = 0; i < num_specs; i++) {
        __CLSpecEnv *env = &work.envs[i];
        if (env->output_buffer) {
            fprintf(stderr, "%s", env->output_buffer);
            free(env->output_buffer);
        }
        if (env->failed) {
            failed++;
        }
    }
    free(work.envs);
    if (__cl_teardown_once) {
        __cl_teardown_once();
    }
    size_t passed = num_specs - failed;
    double elapsed = __cl_time() - time_start;

    if (passed > 0 && failed == 0) {
        __cl_print(__CL_BOLD __CL_FG(__CL_GREEN) "[SUCCESS] " __CL_RESET);
        __cl_print(__CL_FG(__CL_GREEN) "%zu/%zu SPECS failed" __CL_RESET ", "
                           __CL_FG(__CL_GREEN) "%zu/%zu SPECS passed" __CL_RESET
                           " in %.4f s\n\n", failed, num_specs,
                   passed, num_specs, elapsed);
    } else {
        __cl_print(__CL_BOLD __CL_FG(__CL_RED) "[FAILURE] " __CL_RESET);
        __cl_print(__CL_FG(__CL_RED) "%zu/%zu SPECS failed" __CL_RESET ", "
                           __CL_FG(__CL_RED) "%zu/%zu SPECS passed" __CL_RESET
                           " in %.4f s\n\n", failed, num_specs,
                   passed, num_specs, elapsed);
    }
    return failed;
}

/**
 * Create a SPEC bundle with a custom job pool size.
 */
#define CL_BUNDLE_PARALLEL_JOBS(jobs, ...)                                   \
int main(int argc, char** argv) {                                            \
  __CLSpecType specs[] = {__VA_ARGS__};                                      \
  size_t num_specs = sizeof(specs) / sizeof(__CLSpecType);                   \
  return __cl_main_parallel(argc, argv, (jobs), specs, num_specs, __FILE__); \
}                                                                            \

/**
 * Create a SPEC bundle with a sane job pool size.
 */
#define CL_BUNDLE_PARALLEL(...) CL_BUNDLE_PARALLEL_JOBS(4, __VA_ARGS__)

/**
 * Create a SPEC bundle with a job pool size of 1.
 */
#define CL_BUNDLE(...) CL_BUNDLE_PARALLEL_JOBS(1, __VA_ARGS__)

#endif /* CHLORINE2 */