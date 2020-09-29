/* EVMC: Ethereum Client-VM Connector API.
 * Copyright 2019-2020 The EVMC Authors.
 * Licensed under the Apache License, Version 2.0.
 */

#include <assert.h>
#include <stdlib.h>

#include "host.h"

static JavaVM* jvm;

int evmc_java_set_jvm(JNIEnv* jenv)
{
    return (*jenv)->GetJavaVM(jenv, &jvm);
}

static JNIEnv* attach()
{
    JNIEnv* jenv;
    jint rs = (*jvm)->AttachCurrentThread(jvm, (void**)&jenv, NULL);
    (void)rs;
    assert(rs == JNI_OK);
    assert(jenv != NULL);
    return jenv;
}

// Why isn't this helper part of JNI?
static jbyteArray CopyDataToJava(JNIEnv* jenv, const void* ptr, size_t size)
{
    jbyteArray ret = (*jenv)->NewByteArray(jenv, (jsize)size);
    assert(ret != NULL);
    (*jenv)->SetByteArrayRegion(jenv, ret, 0, (jsize)size, (jbyte*)ptr);
    return ret;
}

static bool account_exists_fn(struct evmc_host_context* context, const evmc_address* address)
{
    const char java_method_name[] = "account_exists";
    const char java_method_signature[] = "(I[B)I";

    assert(context != NULL);
    JNIEnv* jenv = attach();

    // get java class
    jclass host_class = (*jenv)->FindClass(jenv, "org/ethereum/evmc/Host");
    assert(host_class != NULL);

    // get java method
    jmethodID method =
        (*jenv)->GetStaticMethodID(jenv, host_class, java_method_name, java_method_signature);
    assert(method != NULL);

    // set java method params
    jbyteArray jaddress = CopyDataToJava(jenv, address, sizeof(struct evmc_address));

    // call java method
    jint jresult = (*jenv)->CallStaticIntMethod(jenv, host_class, method, context->index, jaddress);
    return jresult != 0;
}

static evmc_bytes32 get_storage_fn(struct evmc_host_context* context,
                                   const evmc_address* address,
                                   const evmc_bytes32* key)
{
    const char java_method_name[] = "get_storage";
    const char java_method_signature[] = "(I[B[B)Ljava/nio/ByteBuffer;";

    assert(context != NULL);
    JNIEnv* jenv = attach();

    // get java class
    jclass host_class = (*jenv)->FindClass(jenv, "org/ethereum/evmc/Host");
    assert(host_class != NULL);

    // get java method
    jmethodID method =
        (*jenv)->GetStaticMethodID(jenv, host_class, java_method_name, java_method_signature);
    assert(method != NULL);

    // set java method params
    jbyteArray jaddress = CopyDataToJava(jenv, address, sizeof(struct evmc_address));
    jbyteArray jkey = CopyDataToJava(jenv, key, sizeof(struct evmc_bytes32));

    // call java method
    jobject jresult =
        (*jenv)->CallStaticObjectMethod(jenv, host_class, method, context->index, jaddress, jkey);

    assert(jresult != NULL);
    evmc_bytes32* result_ptr = (struct evmc_bytes32*)(*jenv)->GetDirectBufferAddress(jenv, jresult);
    assert(result_ptr != NULL);
    return *result_ptr; // copy here
}

static enum evmc_storage_status set_storage_fn(struct evmc_host_context* context,
                                               const evmc_address* address,
                                               const evmc_bytes32* key,
                                               const evmc_bytes32* value)
{
    const char java_method_name[] = "set_storage";
    const char java_method_signature[] = "(I[B[B[B)I";

    assert(context != NULL);
    JNIEnv* jenv = attach();

    // get java class
    jclass host_class = (*jenv)->FindClass(jenv, "org/ethereum/evmc/Host");
    assert(host_class != NULL);

    // get java method
    jmethodID method =
        (*jenv)->GetStaticMethodID(jenv, host_class, java_method_name, java_method_signature);
    assert(method != NULL);

    // set java method params
    jbyteArray jaddress = CopyDataToJava(jenv, address, sizeof(struct evmc_address));
    jbyteArray jkey = CopyDataToJava(jenv, key, sizeof(struct evmc_bytes32));
    jbyteArray jval = CopyDataToJava(jenv, value, sizeof(struct evmc_bytes32));

    // call java method
    jint jresult = (*jenv)->CallStaticIntMethod(jenv, host_class, method, context->index, jaddress,
                                                jkey, jval);
    return (enum evmc_storage_status)jresult;
}

static evmc_uint256be get_balance_fn(struct evmc_host_context* context, const evmc_address* address)
{
    const char java_method_name[] = "get_balance";
    const char java_method_signature[] = "(I[B)Ljava/nio/ByteBuffer;";

    assert(context != NULL);
    JNIEnv* jenv = attach();

    // get java class
    jclass host_class = (*jenv)->FindClass(jenv, "org/ethereum/evmc/Host");
    assert(host_class != NULL);

    // get java method
    jmethodID method =
        (*jenv)->GetStaticMethodID(jenv, host_class, java_method_name, java_method_signature);
    assert(method != NULL);

    // set java method params
    jbyteArray jaddress = CopyDataToJava(jenv, address, sizeof(struct evmc_address));

    // call java method
    jobject jresult =
        (*jenv)->CallStaticObjectMethod(jenv, host_class, method, context->index, jaddress);
    assert(jresult != NULL);

    evmc_uint256be* result_ptr = (evmc_uint256be*)(*jenv)->GetDirectBufferAddress(jenv, jresult);
    assert(result_ptr != NULL);
    evmc_uint256be result = *result_ptr;  // copy here

    (*jenv)->ReleaseByteArrayElements(jenv, jaddress, (jbyte*)address, 0);

    return result;
}

static size_t get_code_size_fn(struct evmc_host_context* context, const evmc_address* address)
{
    const char java_method_name[] = "get_code_size";
    const char java_method_signature[] = "(I[B)I";

    assert(context != NULL);
    JNIEnv* jenv = attach();

    // get java class
    jclass host_class = (*jenv)->FindClass(jenv, "org/ethereum/evmc/Host");
    assert(host_class != NULL);

    // get java method
    jmethodID method =
        (*jenv)->GetStaticMethodID(jenv, host_class, java_method_name, java_method_signature);
    assert(method != NULL);

    // set java method params
    jbyteArray jaddress = CopyDataToJava(jenv, address, sizeof(struct evmc_address));

    // call java method
    jint jresult = (*jenv)->CallStaticIntMethod(jenv, host_class, method, context->index, jaddress);
    return (size_t)jresult;
}

static evmc_bytes32 get_code_hash_fn(struct evmc_host_context* context, const evmc_address* address)
{
    const char java_method_name[] = "get_code_hash";
    const char java_method_signature[] = "(I[B)Ljava/nio/ByteBuffer;";

    assert(context != NULL);
    JNIEnv* jenv = attach();

    // get java class
    jclass host_class = (*jenv)->FindClass(jenv, "org/ethereum/evmc/Host");
    assert(host_class != NULL);

    // get java method
    jmethodID method =
        (*jenv)->GetStaticMethodID(jenv, host_class, java_method_name, java_method_signature);
    assert(method != NULL);

    // set java method params
    jbyteArray jaddress = CopyDataToJava(jenv, address, sizeof(struct evmc_address));

    // call java method
    jobject jresult =
        (*jenv)->CallStaticObjectMethod(jenv, host_class, method, context->index, jaddress);
    assert(jresult != NULL);

    evmc_bytes32* result_ptr = (struct evmc_bytes32*)(*jenv)->GetDirectBufferAddress(jenv, jresult);
    assert(result_ptr != NULL);
    evmc_bytes32 result = *result_ptr;  // copy here

    (*jenv)->ReleaseByteArrayElements(jenv, jaddress, (jbyte*)address, 0);

    return result;
}

static size_t copy_code_fn(struct evmc_host_context* context,
                           const evmc_address* address,
                           size_t code_offset,
                           uint8_t* buffer_data,
                           size_t buffer_size)
{
    (void)buffer_size;  // FIXME: buffer_size suspiciously unused.
    const char java_method_name[] = "copy_code";
    const char java_method_signature[] = "(I[BI)Ljava/nio/ByteBuffer;";

    assert(context != NULL);
    JNIEnv* jenv = attach();

    // get java class
    jclass host_class = (*jenv)->FindClass(jenv, "org/ethereum/evmc/Host");
    assert(host_class != NULL);

    // get java method
    jmethodID method =
        (*jenv)->GetStaticMethodID(jenv, host_class, java_method_name, java_method_signature);
    assert(method != NULL);

    // set java method params
    jbyteArray jaddress = CopyDataToJava(jenv, address, sizeof(struct evmc_address));
    jint jcode_offset = (jint)code_offset;

    // call java method
    jobject jresult = (*jenv)->CallStaticObjectMethod(jenv, host_class, method, context->index,
                                                      jaddress, jcode_offset);
    assert(jresult != NULL);

    // copy jresult back to buffer_data
    buffer_data = (uint8_t*)(*jenv)->GetDirectBufferAddress(jenv, jresult);
    (void)buffer_data;
    assert(buffer_data != NULL);

    size_t result = get_code_size_fn(context, address) - code_offset;

    (*jenv)->ReleaseByteArrayElements(jenv, jaddress, (jbyte*)address, 0);

    return result;
}

static void selfdestruct_fn(struct evmc_host_context* context,
                            const evmc_address* address,
                            const evmc_address* beneficiary)
{
    const char java_method_name[] = "selfdestruct";
    const char java_method_signature[] = "(I[B[B)V";

    assert(context != NULL);
    JNIEnv* jenv = attach();

    // get java class
    jclass host_class = (*jenv)->FindClass(jenv, "org/ethereum/evmc/Host");
    assert(host_class != NULL);

    // get java method
    jmethodID method =
        (*jenv)->GetStaticMethodID(jenv, host_class, java_method_name, java_method_signature);
    assert(method != NULL);

    // set java method params
    jbyteArray jaddress = CopyDataToJava(jenv, address, sizeof(struct evmc_address));
    jbyteArray jbeneficiary = CopyDataToJava(jenv, beneficiary, sizeof(struct evmc_address));

    // call java method
    (*jenv)->CallStaticIntMethod(jenv, host_class, method, context->index, jaddress, jbeneficiary);
}

static struct evmc_result call_fn(struct evmc_host_context* context, const struct evmc_message* msg)
{
    const char java_method_name[] = "call";
    const char java_method_signature[] = "(ILjava/nio/ByteBuffer;)Ljava/nio/ByteBuffer;";

    assert(context != NULL);
    JNIEnv* jenv = attach();

    // get java class
    jclass host_class = (*jenv)->FindClass(jenv, "org/ethereum/evmc/Host");
    assert(host_class != NULL);

    // get java method
    jmethodID method =
        (*jenv)->GetStaticMethodID(jenv, host_class, java_method_name, java_method_signature);
    assert(method != NULL);

    // set java method params
    jobject jmsg = (*jenv)->NewDirectByteBuffer(jenv, (void*)msg, sizeof(struct evmc_message));
    assert(jmsg != NULL);

    // call java method
    jobject jresult =
        (*jenv)->CallStaticObjectMethod(jenv, host_class, method, context->index, jmsg);
    assert(jresult != NULL);

    struct evmc_result* result_ptr =
        (struct evmc_result*)(*jenv)->GetDirectBufferAddress(jenv, jresult);
    assert(result_ptr != NULL);
    return *result_ptr; // copy here
}

static struct evmc_tx_context get_tx_context_fn(struct evmc_host_context* context)
{
    const char java_method_name[] = "get_tx_context";
    const char java_method_signature[] = "(I)Ljava/nio/ByteBuffer;";

    assert(context != NULL);
    JNIEnv* jenv = attach();

    // get java class
    jclass host_class = (*jenv)->FindClass(jenv, "org/ethereum/evmc/Host");
    assert(host_class != NULL);

    // get java method
    jmethodID method =
        (*jenv)->GetStaticMethodID(jenv, host_class, java_method_name, java_method_signature);
    assert(method != NULL);

    // call java method
    jobject jresult = (*jenv)->CallStaticObjectMethod(jenv, host_class, method, context->index);
    assert(jresult != NULL);

    struct evmc_tx_context* result_ptr =
        (struct evmc_tx_context*)(*jenv)->GetDirectBufferAddress(jenv, jresult);
    assert(result_ptr != NULL);
    return *result_ptr; // copy here
}

static evmc_bytes32 get_block_hash_fn(struct evmc_host_context* context, int64_t number)
{
    char java_method_name[] = "get_code_hash";
    char java_method_signature[] = "(IJ)Ljava/nio/ByteBuffer;";

    assert(context != NULL);
    JNIEnv* jenv = attach();

    // get java class
    jclass host_class = (*jenv)->FindClass(jenv, "org/ethereum/evmc/Host");
    assert(host_class != NULL);

    // get java method
    jmethodID method =
        (*jenv)->GetStaticMethodID(jenv, host_class, java_method_name, java_method_signature);
    assert(method != NULL);

    // call java method
    jobject jresult =
        (*jenv)->CallStaticObjectMethod(jenv, host_class, method, context->index, (jlong)number);
    assert(jresult != NULL);

    evmc_bytes32* result_ptr = (struct evmc_bytes32*)(*jenv)->GetDirectBufferAddress(jenv, jresult);
    assert(result_ptr != NULL);
    return *result_ptr; // copy here
}

static void emit_log_fn(struct evmc_host_context* context,
                        const evmc_address* address,
                        const uint8_t* data,
                        size_t data_size,
                        const evmc_bytes32 topics[],
                        size_t topics_count)
{
    const char java_method_name[] = "emit_log";
    const char java_method_signature[] = "(I[B[BI[[BI)V";

    assert(context != NULL);
    JNIEnv* jenv = attach();

    // get java class
    jclass host_class = (*jenv)->FindClass(jenv, "org/ethereum/evmc/Host");
    assert(host_class != NULL);

    // get java method
    jmethodID method =
        (*jenv)->GetStaticMethodID(jenv, host_class, java_method_name, java_method_signature);
    assert(method != NULL);

    // set java method params
    jbyteArray jaddress = CopyDataToJava(jenv, address, sizeof(struct evmc_address));
    jbyteArray jdata = CopyDataToJava(jenv, data, data_size);

    jclass byte_type = (*jenv)->FindClass(jenv, "[B");
    jobjectArray jtopics = (*jenv)->NewObjectArray(jenv, (jsize)topics_count, byte_type, NULL);
    assert(jtopics != NULL);
    for (size_t i = 0; i < topics_count; i++)
    {
        jbyteArray jtopic = CopyDataToJava(jenv, topics[i].bytes, sizeof(struct evmc_bytes32));
        (*jenv)->SetObjectArrayElement(jenv, jtopics, (jsize)i, jtopic);
        (*jenv)->DeleteLocalRef(jenv, jtopic);
    }

    // call java method
    (*jenv)->CallStaticIntMethod(jenv, host_class, method, context->index, jaddress, jdata,
                                 data_size, jtopics, topics_count);
}

const struct evmc_host_interface* evmc_java_get_host_interface()
{
    static const struct evmc_host_interface host = {
        account_exists_fn, get_storage_fn,    set_storage_fn,    get_balance_fn,
        get_code_size_fn,  get_code_hash_fn,  copy_code_fn,      selfdestruct_fn,
        call_fn,           get_tx_context_fn, get_block_hash_fn, emit_log_fn,
    };
    return &host;
}
