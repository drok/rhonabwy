/* Public domain, no copyright. Use at your own risk. */

#include <stdio.h>

#include <check.h>
#include <yder.h>
#include <orcania.h>
#include <rhonabwy.h>

#define JSON_JWK_3_1_EC_PUBLIC "cookbook-master/jwk/3_1.ec_public_key.json"
#define JSON_JWK_3_2_EC_PRIVATE "cookbook-master/jwk/3_2.ec_private_key.json"
#define JSON_JWK_3_3_RSA_PUBLIC "cookbook-master/jwk/3_3.rsa_public_key.json"
#define JSON_JWK_3_4_RSA_PRIVATE "cookbook-master/jwk/3_4.rsa_private_key.json"
#define JSON_JWK_3_5_SYMMETRIC_MAC "cookbook-master/jwk/3_5.symmetric_key_mac_computation.json"
#define JSON_JWK_3_6_SYMMETRIC_ENC "cookbook-master/jwk/3_6.symmetric_key_encryption.json"

#define JSON_JWS_4_1_RSA_V15 "cookbook-master/jws/4_1.rsa_v15_signature.json"
#define JSON_JWS_4_2_RSA_PSS "cookbook-master/jws/4_2.rsa-pss_signature.json"
#define JSON_JWS_4_3_ECDSA "cookbook-master/jws/4_3.ecdsa_signature.json"
#define JSON_JWS_4_4_HMAC_SHA2 "cookbook-master/jws/4_4.hmac-sha2_integrity_protection.json"
#define JSON_JWS_4_5_DETACHED "cookbook-master/jws/4_5.signature_with_detached_content.json"
#define JSON_JWS_4_6_PROTECTING_HEADER "cookbook-master/jws/4_6.protecting_specific_header_fields.json"
#define JSON_JWS_4_7_PROTECTING_CONTENT "cookbook-master/jws/4_7.protecting_content_only.json"
#define JSON_JWS_4_8_MULTIPLE_SIGNATURES "cookbook-master/jws/4_8.multiple_signatures.json"

#define JSON_JWE_5_1_RSA_V15_AES_CBC "cookbook-master/jwe/5_1.key_encryption_using_rsa_v15_and_aes-hmac-sha2.json"
#define JSON_JWE_5_2_RSA_OAEP_AES_GCM "cookbook-master/jwe/5_2.key_encryption_using_rsa-oaep_with_aes-gcm.json"
#define JSON_JWE_5_3_PBES2_AES_CBC "cookbook-master/jwe/5_3.key_wrap_using_pbes2-aes-keywrap_with-aes-cbc-hmac-sha2.json"
#define JSON_JWE_5_4_ECDH_ES_AES_GCM "cookbook-master/jwe/5_4.key_agreement_with_key_wrapping_using_ecdh-es_and_aes-keywrap_with_aes-gcm.json"
#define JSON_JWE_5_5_ECDH_ES_AES_CBC "cookbook-master/jwe/5_5.key_agreement_using_ecdh-es_with_aes-cbc-hmac-sha2.json"
#define JSON_JWE_5_6_DIR_AES_GCM "cookbook-master/jwe/5_6.direct_encryption_using_aes-gcm.json"
#define JSON_JWE_5_7_AES_GCM_KEYWRAP_AES_CBC "cookbook-master/jwe/5_7.key_wrap_using_aes-gcm_keywrap_with_aes-cbc-hmac-sha2.json"
#define JSON_JWE_5_8_AES_KEYWRAP_AES_GCM "cookbook-master/jwe/5_8.key_wrap_using_aes-keywrap_with_aes-gcm.json"
#define JSON_JWE_5_9_COMPRESSED "cookbook-master/jwe/5_9.compressed_content.json"
#define JSON_JWE_5_10_AAD "cookbook-master/jwe/5_10.including_additional_authentication_data.json"
#define JSON_JWE_5_11_PROTECTING_HEADER "cookbook-master/jwe/5_11.protecting_specific_header_fields.json"
#define JSON_JWE_5_12_PROTECTING_CONTENT "cookbook-master/jwe/5_12.protecting_content_only.json"
#define JSON_JWE_5_13_MULTIPLE_RECIPIENTS "cookbook-master/jwe/5_13.encrypting_to_multiple_recipients.json"

#define JSON_JWS_CURVE25519 "cookbook-master/curve25519/jws.json"
#define JSON_JWE_CURVE25519 "cookbook-master/curve25519/ecdh-es.json"

static char * get_file_content(const char * file_path) {
  char * buffer = NULL;
  size_t length, res;
  FILE * f;

  f = fopen (file_path, "rb");
  if (f) {
    fseek (f, 0, SEEK_END);
    length = ftell (f);
    fseek (f, 0, SEEK_SET);
    buffer = o_malloc((length+1)*sizeof(char));
    if (buffer) {
      res = fread (buffer, 1, length, f);
      if (res != length) {
        fprintf(stderr, "fread warning, reading %zu while expecting %zu", res, length);
      }
      // Add null character at the end of buffer, just in case
      buffer[length] = '\0';
    }
    fclose (f);
  } else {
    fprintf(stderr, "error opening file %s\n", file_path);
  }
  
  return buffer;
}

static void jws_test(const char * file_path) {
  char * file_content, * token;
  json_t * j_content;
  jws_t * jws;
  jwk_t * jwk_privkey, * jwk_pubkey;
  
  ck_assert_ptr_ne(NULL, file_content = get_file_content(file_path));
  ck_assert_ptr_ne(NULL, j_content = json_loads(file_content, JSON_DECODE_ANY, NULL));
  
  y_log_message(Y_LOG_LEVEL_INFO, "Run test: %s", json_string_value(json_object_get(j_content, "title")));
  
  ck_assert_int_eq(r_jwk_init(&jwk_privkey), RHN_OK);
  ck_assert_int_eq(r_jwk_init(&jwk_pubkey), RHN_OK);
  ck_assert_int_eq(r_jwk_import_from_json_t(jwk_privkey, json_object_get(json_object_get(j_content, "input"), "key")), RHN_OK);
  if (r_jwk_key_type(jwk_privkey, NULL, 0) & R_KEY_TYPE_PRIVATE) {
    ck_assert_int_eq(r_jwk_extract_pubkey(jwk_privkey, jwk_pubkey, 0), RHN_OK);
  } else if (r_jwk_key_type(jwk_privkey, NULL, 0) & R_KEY_TYPE_SYMMETRIC) {
    ck_assert_int_eq(r_jwk_import_from_json_t(jwk_pubkey, json_object_get(json_object_get(j_content, "input"), "key")), RHN_OK);
  }
  
  if (json_object_get(j_content, "reproducible") == json_true()) {
    ck_assert_int_eq(r_jws_init(&jws), RHN_OK);
    ck_assert_int_eq(r_jws_set_payload(jws, (const unsigned char *)json_string_value(json_object_get(json_object_get(j_content, "input"), "payload")), json_string_length(json_object_get(json_object_get(j_content, "input"), "payload"))), RHN_OK);
    ck_assert_int_eq(r_jws_set_full_header_json_t(jws, json_object_get(json_object_get(j_content, "signing"), "protected")), RHN_OK);
    ck_assert_int_eq(r_jws_add_keys_json_t(jws, jwk_privkey, NULL), RHN_OK);
    ck_assert_ptr_ne(NULL, token = r_jws_serialize(jws, NULL, 0));
    ck_assert_str_eq(token, json_string_value(json_object_get(json_object_get(j_content, "output"), "compact")));
    r_jws_free(jws);
    o_free(token);
  }
  
  ck_assert_int_eq(r_jws_init(&jws), RHN_OK);
  ck_assert_int_eq(r_jws_parse(jws, json_string_value(json_object_get(json_object_get(j_content, "output"), "compact")), 0), RHN_OK);
  ck_assert_int_eq(r_jws_add_keys_json_t(jws, NULL, jwk_pubkey), RHN_OK);
  ck_assert_int_eq(r_jws_verify_signature(jws, NULL, 0), RHN_OK);
  r_jws_free(jws);

  r_jwk_free(jwk_privkey);
  r_jwk_free(jwk_pubkey);
  json_decref(j_content);
  o_free(file_content);
}

static void jwe_test(const char * file_path) {
  char * file_content, * token;
  json_t * j_content, * j_json;
  jwe_t * jwe, * jwe_c, * jwe_f, * jwe_g;
  jwk_t * jwk_privkey, * jwk_pubkey;
  jwks_t * jwks_privkey, * jwks_pubkey;
  unsigned char iv[128] = {0}, cek[128] = {0};
  const unsigned char * payload;
  size_t iv_len = 0, cek_len = 0, payload_len = 0;
  
  ck_assert_ptr_ne(NULL, file_content = get_file_content(file_path));
  ck_assert_ptr_ne(NULL, j_content = json_loads(file_content, JSON_DECODE_ANY, NULL));
  
  y_log_message(Y_LOG_LEVEL_INFO, "Run test: %s", json_string_value(json_object_get(j_content, "title")));
  
  ck_assert_int_eq(r_jwk_init(&jwk_privkey), RHN_OK);
  ck_assert_int_eq(r_jwks_init(&jwks_privkey), RHN_OK);
  ck_assert_int_eq(r_jwks_init(&jwks_pubkey), RHN_OK);
  if (json_object_get(json_object_get(j_content, "input"), "key") != NULL) {
    ck_assert_int_eq(r_jwk_import_from_json_t(jwk_privkey, json_object_get(json_object_get(j_content, "input"), "key")), RHN_OK);
  } else if (json_object_get(json_object_get(j_content, "input"), "pwd") != NULL) {
    ck_assert_int_eq(r_jwk_import_from_symmetric_key(jwk_privkey, (const unsigned char *)json_string_value(json_object_get(json_object_get(j_content, "input"), "pwd")), json_string_length(json_object_get(json_object_get(j_content, "input"), "pwd"))), RHN_OK);
  }
  if (r_jwk_key_type(jwk_privkey, NULL, 0) & R_KEY_TYPE_PRIVATE) {
    ck_assert_int_eq(r_jwk_init(&jwk_pubkey), RHN_OK);
    ck_assert_int_eq(r_jwk_extract_pubkey(jwk_privkey, jwk_pubkey, 0), RHN_OK);
  } else if (r_jwk_key_type(jwk_privkey, NULL, 0) & R_KEY_TYPE_SYMMETRIC) {
    ck_assert_ptr_ne(NULL, jwk_pubkey = r_jwk_copy(jwk_privkey));
  }
  ck_assert_int_eq(r_jwks_append_jwk(jwks_privkey, jwk_privkey), RHN_OK);
  ck_assert_int_eq(r_jwks_append_jwk(jwks_pubkey, jwk_pubkey), RHN_OK);
  
  if (json_object_get(j_content, "reproducible") == json_true()) {
    ck_assert_int_eq(r_jwe_init(&jwe_c), RHN_OK);
    ck_assert_int_eq(r_jwe_set_payload(jwe_c, (const unsigned char *)json_string_value(json_object_get(json_object_get(j_content, "input"), "plaintext")), json_string_length(json_object_get(json_object_get(j_content, "input"), "plaintext"))), RHN_OK);
    if (json_object_get(json_object_get(j_content, "input"), "alg")) {
      ck_assert_int_eq(r_jwe_set_alg(jwe_c, r_str_to_jwa_alg(json_string_value(json_object_get(json_object_get(j_content, "input"), "alg")))), RHN_OK);
    }
    if (json_object_get(json_object_get(j_content, "input"), "enc")) {
      ck_assert_int_eq(r_jwe_set_enc(jwe_c, r_str_to_jwa_enc(json_string_value(json_object_get(json_object_get(j_content, "input"), "enc")))), RHN_OK);
    }
    if (json_object_get(json_object_get(j_content, "encrypting_content"), "protected") != NULL) {
      ck_assert_int_eq(r_jwe_set_full_header_json_t(jwe_c, json_object_get(json_object_get(j_content, "encrypting_content"), "protected")), RHN_OK);
    }
    ck_assert_int_eq(1, o_base64url_decode((const unsigned char *)json_string_value(json_object_get(json_object_get(j_content, "generated"), "iv")), json_string_length(json_object_get(json_object_get(j_content, "generated"), "iv")), iv, &iv_len));
    ck_assert_int_eq(r_jwe_set_iv(jwe_c, iv, iv_len), RHN_OK);
    if (json_object_get(json_object_get(j_content, "generated"), "cek") != NULL) {
      ck_assert_int_eq(1, o_base64url_decode((const unsigned char *)json_string_value(json_object_get(json_object_get(j_content, "generated"), "cek")), json_string_length(json_object_get(json_object_get(j_content, "generated"), "cek")), cek, &cek_len));
      ck_assert_int_eq(r_jwe_set_cypher_key(jwe_c, cek, cek_len), RHN_OK);
    }
    if (json_object_get(json_object_get(j_content, "input"), "aad") != NULL) {
      ck_assert_int_eq(r_jwe_set_aad(jwe_c, (const unsigned char *)json_string_value(json_object_get(json_object_get(j_content, "input"), "aad")), json_string_length(json_object_get(json_object_get(j_content, "input"), "aad"))), RHN_OK);
    }
    if (json_object_get(json_object_get(j_content, "encrypting_content"), "unprotected") != NULL) {
      ck_assert_int_eq(r_jwe_set_full_unprotected_header_json_t(jwe_c, json_object_get(json_object_get(j_content, "encrypting_content"), "unprotected")), RHN_OK);
    }
    ck_assert_ptr_ne(NULL, jwe_f = r_jwe_copy(jwe_c));
    ck_assert_ptr_ne(NULL, jwe_g = r_jwe_copy(jwe_c));
    
    if (json_object_get(json_object_get(j_content, "output"), "compact") != NULL) {
      ck_assert_ptr_ne(NULL, token = r_jwe_serialize(jwe_c, jwk_pubkey, 0));
      ck_assert_str_eq(token, json_string_value(json_object_get(json_object_get(j_content, "output"), "compact")));
      o_free(token);
    }

    if (json_object_get(json_object_get(j_content, "output"), "json_flat") != NULL) {
      ck_assert_ptr_ne(NULL, j_json = r_jwe_serialize_json_t(jwe_f, jwks_pubkey, 0, R_JSON_MODE_FLATTENED));
      //y_log_message(Y_LOG_LEVEL_DEBUG, "j_json %s", json_dumps(j_json, JSON_INDENT(2)));
      //y_log_message(Y_LOG_LEVEL_DEBUG, "reference %s", json_dumps(json_object_get(json_object_get(j_content, "output"), "json_flat"), JSON_INDENT(2)));
      
      ck_assert_int_eq(1, json_equal(j_json, json_object_get(json_object_get(j_content, "output"), "json_flat")));
      json_decref(j_json);
    }
    
    if (json_object_get(json_object_get(j_content, "output"), "json") != NULL) {
      ck_assert_ptr_ne(NULL, j_json = r_jwe_serialize_json_t(jwe_g, jwks_pubkey, 0, R_JSON_MODE_GENERAL));
      //y_log_message(Y_LOG_LEVEL_DEBUG, "j_json %s", json_dumps(j_json, JSON_INDENT(2)));
      //y_log_message(Y_LOG_LEVEL_DEBUG, "reference %s", json_dumps(json_object_get(json_object_get(j_content, "output"), "json"), JSON_INDENT(2)));
      
      ck_assert_int_eq(1, json_equal(j_json, json_object_get(json_object_get(j_content, "output"), "json")));
      json_decref(j_json);
    }
    
    r_jwe_free(jwe_c);
    r_jwe_free(jwe_f);
    r_jwe_free(jwe_g);
  }
  
  if (json_object_get(json_object_get(j_content, "output"), "compact") != NULL) {
    ck_assert_int_eq(r_jwe_init(&jwe), RHN_OK);
    ck_assert_int_eq(r_jwe_parse(jwe, json_string_value(json_object_get(json_object_get(j_content, "output"), "compact")), 0), RHN_OK);
    ck_assert_int_eq(r_jwe_decrypt(jwe, jwk_privkey, 0), RHN_OK);
    ck_assert_ptr_ne(NULL, payload = r_jwe_get_payload(jwe, &payload_len));
    ck_assert_int_eq(payload_len, json_string_length(json_object_get(json_object_get(j_content, "input"), "plaintext")));
    ck_assert_int_eq(0, memcmp(payload, json_string_value(json_object_get(json_object_get(j_content, "input"), "plaintext")), payload_len));
    r_jwe_free(jwe);
  }

  if (json_object_get(json_object_get(j_content, "output"), "json") != NULL) {
    ck_assert_int_eq(r_jwe_init(&jwe), RHN_OK);
    ck_assert_int_eq(r_jwe_parse_json_t(jwe, json_object_get(json_object_get(j_content, "output"), "json"), 0), RHN_OK);
    ck_assert_int_eq(r_jwe_decrypt(jwe, jwk_privkey, 0), RHN_OK);
    ck_assert_ptr_ne(NULL, payload = r_jwe_get_payload(jwe, &payload_len));
    ck_assert_int_eq(payload_len, json_string_length(json_object_get(json_object_get(j_content, "input"), "plaintext")));
    ck_assert_int_eq(0, memcmp(payload, json_string_value(json_object_get(json_object_get(j_content, "input"), "plaintext")), payload_len));
    r_jwe_free(jwe);
  }

  if (json_object_get(json_object_get(j_content, "output"), "json_flat") != NULL) {
    ck_assert_int_eq(r_jwe_init(&jwe), RHN_OK);
    ck_assert_int_eq(r_jwe_parse_json_t(jwe, json_object_get(json_object_get(j_content, "output"), "json_flat"), 0), RHN_OK);
    ck_assert_int_eq(r_jwe_decrypt(jwe, jwk_privkey, 0), RHN_OK);
    ck_assert_ptr_ne(NULL, payload = r_jwe_get_payload(jwe, &payload_len));
    ck_assert_int_eq(payload_len, json_string_length(json_object_get(json_object_get(j_content, "input"), "plaintext")));
    ck_assert_int_eq(0, memcmp(payload, json_string_value(json_object_get(json_object_get(j_content, "input"), "plaintext")), payload_len));
    r_jwe_free(jwe);
  }

  r_jwk_free(jwk_privkey);
  r_jwk_free(jwk_pubkey);
  r_jwks_free(jwks_privkey);
  r_jwks_free(jwks_pubkey);
  json_decref(j_content);
  o_free(file_content);
}

START_TEST(test_rhonabwy_cookbook_jwk)
{
  char * file_content;
  jwk_t * jwk;
  unsigned int bits = 0;
  y_log_message(Y_LOG_LEVEL_INFO, "Test JWK");
  
  y_log_message(Y_LOG_LEVEL_INFO, "Run test: %s", o_strrchr(JSON_JWK_3_1_EC_PUBLIC, '/')+1);
  ck_assert_ptr_ne(NULL, file_content = get_file_content(JSON_JWK_3_1_EC_PUBLIC));
  ck_assert_int_eq(r_jwk_init(&jwk), RHN_OK);
  ck_assert_int_eq(r_jwk_import_from_json_str(jwk, file_content), RHN_OK);
  ck_assert_int_eq(r_jwk_key_type(jwk, &bits, 0), R_KEY_TYPE_EC|R_KEY_TYPE_PUBLIC);
  ck_assert_int_eq(521, bits);
  r_jwk_free(jwk);
  o_free(file_content);
  
  y_log_message(Y_LOG_LEVEL_INFO, "Run test: %s", o_strrchr(JSON_JWK_3_2_EC_PRIVATE, '/')+1);
  ck_assert_ptr_ne(NULL, file_content = get_file_content(JSON_JWK_3_2_EC_PRIVATE));
  ck_assert_int_eq(r_jwk_init(&jwk), RHN_OK);
  ck_assert_int_eq(r_jwk_import_from_json_str(jwk, file_content), RHN_OK);
  ck_assert_int_eq(r_jwk_key_type(jwk, &bits, 0), R_KEY_TYPE_EC|R_KEY_TYPE_PRIVATE);
  ck_assert_int_eq(521, bits);
  r_jwk_free(jwk);
  o_free(file_content);
  
  y_log_message(Y_LOG_LEVEL_INFO, "Run test: %s", o_strrchr(JSON_JWK_3_3_RSA_PUBLIC, '/')+1);
  ck_assert_ptr_ne(NULL, file_content = get_file_content(JSON_JWK_3_3_RSA_PUBLIC));
  ck_assert_int_eq(r_jwk_init(&jwk), RHN_OK);
  ck_assert_int_eq(r_jwk_import_from_json_str(jwk, file_content), RHN_OK);
  ck_assert_int_eq(r_jwk_key_type(jwk, &bits, 0), R_KEY_TYPE_RSA|R_KEY_TYPE_PUBLIC);
  ck_assert_int_eq(2048, bits);
  r_jwk_free(jwk);
  o_free(file_content);
  
  y_log_message(Y_LOG_LEVEL_INFO, "Run test: %s", o_strrchr(JSON_JWK_3_4_RSA_PRIVATE, '/')+1);
  ck_assert_ptr_ne(NULL, file_content = get_file_content(JSON_JWK_3_4_RSA_PRIVATE));
  ck_assert_int_eq(r_jwk_init(&jwk), RHN_OK);
  ck_assert_int_eq(r_jwk_import_from_json_str(jwk, file_content), RHN_OK);
  ck_assert_int_eq(r_jwk_key_type(jwk, &bits, 0), R_KEY_TYPE_RSA|R_KEY_TYPE_PRIVATE);
  ck_assert_int_eq(2048, bits);
  r_jwk_free(jwk);
  o_free(file_content);
  
  y_log_message(Y_LOG_LEVEL_INFO, "Run test: %s", o_strrchr(JSON_JWK_3_5_SYMMETRIC_MAC, '/')+1);
  ck_assert_ptr_ne(NULL, file_content = get_file_content(JSON_JWK_3_5_SYMMETRIC_MAC));
  ck_assert_int_eq(r_jwk_init(&jwk), RHN_OK);
  ck_assert_int_eq(r_jwk_import_from_json_str(jwk, file_content), RHN_OK);
  ck_assert_int_eq(r_jwk_key_type(jwk, &bits, 0), R_KEY_TYPE_HMAC|R_KEY_TYPE_SYMMETRIC);
  ck_assert_int_eq(256, bits);
  r_jwk_free(jwk);
  o_free(file_content);
  
  y_log_message(Y_LOG_LEVEL_INFO, "Run test: %s", o_strrchr(JSON_JWK_3_6_SYMMETRIC_ENC, '/')+1);
  ck_assert_ptr_ne(NULL, file_content = get_file_content(JSON_JWK_3_6_SYMMETRIC_ENC));
  ck_assert_int_eq(r_jwk_init(&jwk), RHN_OK);
  ck_assert_int_eq(r_jwk_import_from_json_str(jwk, file_content), RHN_OK);
  ck_assert_int_eq(r_jwk_key_type(jwk, &bits, 0), R_KEY_TYPE_HMAC|R_KEY_TYPE_SYMMETRIC);
  ck_assert_int_eq(256, bits);
  r_jwk_free(jwk);
  o_free(file_content);
}
END_TEST

START_TEST(test_rhonabwy_cookbook_jws)
{
  y_log_message(Y_LOG_LEVEL_INFO, "Test JWS");
  jws_test(JSON_JWS_4_1_RSA_V15);
#if GNUTLS_VERSION_NUMBER >= 0x030600
  jws_test(JSON_JWS_4_2_RSA_PSS);
  jws_test(JSON_JWS_4_3_ECDSA);
  jws_test(JSON_JWS_CURVE25519);
#endif
  jws_test(JSON_JWS_4_4_HMAC_SHA2);
  //jws_test(JSON_JWS_4_5_DETACHED);
  //jws_test(JSON_JWS_4_6_PROTECTING_HEADER);
  //jws_test(JSON_JWS_4_7_PROTECTING_CONTENT);
  //jws_test(JSON_JWS_4_8_MULTIPLE_SIGNATURES);
}
END_TEST

START_TEST(test_rhonabwy_cookbook_jwe)
{
  y_log_message(Y_LOG_LEVEL_INFO, "Test JWE");
  jwe_test(JSON_JWE_5_1_RSA_V15_AES_CBC); // Not reproductible yet
#if NETTLE_VERSION_NUMBER >= 0x030400
  jwe_test(JSON_JWE_5_2_RSA_OAEP_AES_GCM);
#endif
#if GNUTLS_VERSION_NUMBER >= 0x03060d
  jwe_test(JSON_JWE_5_3_PBES2_AES_CBC);
#endif
#if NETTLE_VERSION_NUMBER >= 0x030600
  jwe_test(JSON_JWE_5_4_ECDH_ES_AES_GCM);
  jwe_test(JSON_JWE_5_5_ECDH_ES_AES_CBC);
#endif
  jwe_test(JSON_JWE_5_6_DIR_AES_GCM);
#if NETTLE_VERSION_NUMBER >= 0x030400
  jwe_test(JSON_JWE_5_7_AES_GCM_KEYWRAP_AES_CBC);
  jwe_test(JSON_JWE_5_8_AES_KEYWRAP_AES_GCM);
  jwe_test(JSON_JWE_5_9_COMPRESSED);
  jwe_test(JSON_JWE_5_10_AAD);
  jwe_test(JSON_JWE_5_11_PROTECTING_HEADER);
  //jwe_test(JSON_JWE_5_12_PROTECTING_CONTENT); // Not sure if relevant
  //jwe_test(JSON_JWE_5_13_MULTIPLE_RECIPIENTS);
#endif
#if NETTLE_VERSION_NUMBER >= 0x030600
  jwe_test(JSON_JWE_CURVE25519);
#endif
}
END_TEST

static Suite *rhonabwy_suite(void)
{
  Suite *s;
  TCase *tc_core;

  s = suite_create("Rhonabwy cookbook tests");
  tc_core = tcase_create("test_rhonabwy_cookbook");
  tcase_add_test(tc_core, test_rhonabwy_cookbook_jwk);
  tcase_add_test(tc_core, test_rhonabwy_cookbook_jws);
  tcase_add_test(tc_core, test_rhonabwy_cookbook_jwe);
  tcase_set_timeout(tc_core, 30);
  suite_add_tcase(s, tc_core);

  return s;
}

int main(int argc, char *argv[])
{
  int number_failed;
  Suite *s;
  SRunner *sr;
  y_init_logs("Rhonabwy", Y_LOG_MODE_CONSOLE, Y_LOG_LEVEL_DEBUG, NULL, "Starting Rhonabwy cookbook tests");
  r_global_init();
  s = rhonabwy_suite();
  sr = srunner_create(s);

  srunner_run_all(sr, CK_VERBOSE);
  number_failed = srunner_ntests_failed(sr);
  srunner_free(sr);
  
  r_global_close();
  y_close_logs();
  return (number_failed == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}
