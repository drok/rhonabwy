/**
 *
 * Rhonabwy JSON Web Signature (JWS) library
 *
 * jws.c: functions definitions
 *
 * Copyright 2020-2022 Nicolas Mora <mail@babelouest.org>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public License
 * as published by the Free Software Foundation;
 * version 2.1 of the License.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU GENERAL PUBLIC LICENSE for more details.
 *
 * You should have received a copy of the GNU General Public
 * License along with this library.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#include <string.h>
#include <ctype.h>
#include <gnutls/gnutls.h>
#include <gnutls/crypto.h>
#include <gnutls/abstract.h>
#include <gnutls/x509.h>
#include <orcania.h>
#include <yder.h>
#include <rhonabwy.h>

static json_t * r_jws_parse_protected(const unsigned char * header_b64url) {
  json_t * j_return = NULL;
  struct _o_datum dat = {0, NULL};

  do {
    if (!o_base64url_decode_alloc(header_b64url, o_strlen((const char *)header_b64url), &dat)) {
      y_log_message(Y_LOG_LEVEL_ERROR, "r_jws_parse_protected - Invalid base64");
      break;
    }
    j_return = json_loadb((const char *)dat.data, dat.size, JSON_DECODE_ANY, NULL);
  } while(0);
  o_free(dat.data);
  return j_return;
}

static int r_jws_extract_header(jws_t * jws, json_t * j_header, uint32_t parse_flags, int x5u_flags) {
  int ret;
  jwk_t * jwk;

  if (json_is_object(j_header)) {
    ret = RHN_OK;

    if (json_object_get(j_header, "alg") != NULL) {
      if (0 != o_strcmp("HS256", json_string_value(json_object_get(j_header, "alg"))) && 0 != o_strcmp("HS384", json_string_value(json_object_get(j_header, "alg"))) && 0 != o_strcmp("HS512", json_string_value(json_object_get(j_header, "alg"))) &&
      0 != o_strcmp("RS256", json_string_value(json_object_get(j_header, "alg"))) && 0 != o_strcmp("RS384", json_string_value(json_object_get(j_header, "alg"))) && 0 != o_strcmp("RS512", json_string_value(json_object_get(j_header, "alg"))) &&
      0 != o_strcmp("PS256", json_string_value(json_object_get(j_header, "alg"))) && 0 != o_strcmp("PS384", json_string_value(json_object_get(j_header, "alg"))) && 0 != o_strcmp("PS512", json_string_value(json_object_get(j_header, "alg"))) &&
      0 != o_strcmp("ES256", json_string_value(json_object_get(j_header, "alg"))) && 0 != o_strcmp("ES384", json_string_value(json_object_get(j_header, "alg"))) && 0 != o_strcmp("ES512", json_string_value(json_object_get(j_header, "alg"))) &&
      0 != o_strcmp("EdDSA", json_string_value(json_object_get(j_header, "alg"))) && 0 != o_strcmp("ES256K", json_string_value(json_object_get(j_header, "alg"))) && 0 != o_strcmp("none", json_string_value(json_object_get(j_header, "alg")))) {
        y_log_message(Y_LOG_LEVEL_ERROR, "r_jws_extract_header - Invalid alg");
        ret = RHN_ERROR_PARAM;
      } else {
        jws->alg = r_str_to_jwa_alg(json_string_value(json_object_get(j_header, "alg")));
      }
    }

    if (json_string_length(json_object_get(j_header, "jku")) && (parse_flags&R_PARSE_HEADER_JKU)) {
      if (r_jwks_import_from_uri(jws->jwks_pubkey, json_string_value(json_object_get(j_header, "jku")), x5u_flags) != RHN_OK) {
        y_log_message(Y_LOG_LEVEL_ERROR, "r_jws_extract_header - Error loading jwks from uri %s", json_string_value(json_object_get(j_header, "jku")));
      }
    }

    if (json_object_get(j_header, "jwk") != NULL && (parse_flags&R_PARSE_HEADER_JWK)) {
      r_jwk_init(&jwk);
      if (r_jwk_import_from_json_t(jwk, json_object_get(j_header, "jwk")) == RHN_OK && r_jwk_key_type(jwk, NULL, 0)&R_KEY_TYPE_PUBLIC) {
        if (r_jwks_append_jwk(jws->jwks_pubkey, jwk) != RHN_OK) {
          ret = RHN_ERROR;
        }
      } else {
        y_log_message(Y_LOG_LEVEL_ERROR, "r_jws_extract_header - Error parsing header jwk");
        ret = RHN_ERROR_PARAM;
      }
      r_jwk_free(jwk);
    }

    if (json_object_get(j_header, "x5u") != NULL && (parse_flags&R_PARSE_HEADER_X5U)) {
      r_jwk_init(&jwk);
      if (r_jwk_import_from_x5u(jwk, x5u_flags, json_string_value(json_object_get(j_header, "x5u"))) == RHN_OK) {
        if (r_jwks_append_jwk(jws->jwks_pubkey, jwk) != RHN_OK) {
          ret = RHN_ERROR;
        }
      } else {
        y_log_message(Y_LOG_LEVEL_ERROR, "r_jws_extract_header - Error importing x5u");
        ret = RHN_ERROR_PARAM;
      }
      r_jwk_free(jwk);
    }

    if (json_object_get(j_header, "x5c") != NULL && (parse_flags&R_PARSE_HEADER_X5C)) {
      r_jwk_init(&jwk);
      if (r_jwk_import_from_x5c(jwk, json_string_value(json_array_get(json_object_get(j_header, "x5c"), 0))) == RHN_OK) {
        if (r_jwks_append_jwk(jws->jwks_pubkey, jwk) != RHN_OK) {
          ret = RHN_ERROR;
        }
      } else {
        y_log_message(Y_LOG_LEVEL_ERROR, "r_jws_extract_header - Error importing x5c");
        ret = RHN_ERROR_PARAM;
      }
      r_jwk_free(jwk);
    }
  } else {
    ret = RHN_ERROR_PARAM;
  }

  return ret;
}

static int r_jws_set_header_value(jws_t * jws, int force) {
  int ret = RHN_OK;
  char * header_str = NULL;
  struct _o_datum dat = {0, NULL};

  if (jws != NULL) {
    if (jws->header_b64url == NULL || force) {
      if ((header_str = json_dumps(jws->j_header, JSON_COMPACT)) != NULL) {
        if (o_base64url_encode_alloc((const unsigned char *)header_str, o_strlen(header_str), &dat)) {
          o_free(jws->header_b64url);
          jws->header_b64url = (unsigned char *)o_strndup((const char *)dat.data, dat.size);
          o_free(dat.data);
        } else {
          y_log_message(Y_LOG_LEVEL_ERROR, "r_jws_set_header_value - Error o_base64url_encode header_str");
          ret = RHN_ERROR;
        }
        o_free(header_str);
      } else {
        y_log_message(Y_LOG_LEVEL_ERROR, "r_jws_set_header_value - Error json_dumps header_str");
        ret = RHN_ERROR;
      }
    }
  } else {
    ret = RHN_ERROR_PARAM;
  }
  return ret;
}

static int r_jws_set_payload_value(jws_t * jws, int force) {
  int ret = RHN_OK, zip = 0;
  unsigned char * payload_to_set = NULL;
  size_t payload_to_set_len = 0;
  struct _o_datum dat = {0, NULL};

  if (jws != NULL) {
    if (jws->payload_b64url == NULL || force) {
      if (jws->payload_len) {
        if (0 == o_strcmp("DEF", r_jws_get_header_str_value(jws, "zip"))) {
          zip = 1;
          if ((ret = _r_deflate_payload(jws->payload, jws->payload_len, &payload_to_set, &payload_to_set_len)) != RHN_OK) {
            y_log_message(Y_LOG_LEVEL_ERROR, "r_jws_set_payload_value - Error _r_deflate_payload");
          }
        } else {
          payload_to_set = jws->payload;
          payload_to_set_len = jws->payload_len;
        }
        if (ret == RHN_OK) {
          if (o_base64url_encode_alloc(payload_to_set, payload_to_set_len, &dat)) {
            o_free(jws->payload_b64url);
            jws->payload_b64url = (unsigned char *)o_strndup((const char *)dat.data, dat.size);
            o_free(dat.data);
          } else {
            y_log_message(Y_LOG_LEVEL_ERROR, "r_jws_set_payload_value - Error o_base64url_encode payload");
            ret = RHN_ERROR;
          }
        }
        if (zip) {
          o_free(payload_to_set);
        }
      } else {
        y_log_message(Y_LOG_LEVEL_ERROR, "r_jws_set_payload_value - Error empty payload");
        ret = RHN_ERROR_PARAM;
      }
    }
  } else {
    ret = RHN_ERROR_PARAM;
  }
  return ret;
}

static int r_jws_set_token_values(jws_t * jws, int force) {
  int ret;

  if ((ret = r_jws_set_header_value(jws, force)) == RHN_OK) {
    ret = r_jws_set_payload_value(jws, force);
  }
  return ret;
}

static unsigned char * r_jws_sign_hmac(jws_t * jws, jwk_t * jwk) {
  int alg = GNUTLS_DIG_NULL;
  unsigned char * data = NULL, * key = NULL, * sig = NULL, * to_return = NULL;
  size_t key_len = 0, sig_len = 0;
  struct _o_datum dat_sig = {0, NULL};

  if (jws->alg == R_JWA_ALG_HS256) {
    alg = GNUTLS_DIG_SHA256;
  } else if (jws->alg == R_JWA_ALG_HS384) {
    alg = GNUTLS_DIG_SHA384;
  } else if (jws->alg == R_JWA_ALG_HS512) {
    alg = GNUTLS_DIG_SHA512;
  }

  if (alg != GNUTLS_DIG_NULL) {
    sig_len = (unsigned)gnutls_hmac_get_len(alg);
    sig = o_malloc(sig_len);

    key_len = o_strlen(r_jwk_get_property_str(jwk, "k"));
    if (key_len) {
      key = o_malloc(key_len);

      if (key != NULL) {
        if (r_jwk_export_to_symmetric_key(jwk, key, &key_len) != RHN_OK) {
          y_log_message(Y_LOG_LEVEL_ERROR, "r_jws_sign_hmac - Error r_jwk_export_to_symmetric_key");
          o_free(key);
          key = NULL;
        }
      } else {
        y_log_message(Y_LOG_LEVEL_ERROR, "r_jws_sign_hmac - Error allocating resources for key");
      }
    } else {
      y_log_message(Y_LOG_LEVEL_ERROR, "r_jws_sign_hmac - Error key invalid, 'k' empty");
    }
  } else {
    y_log_message(Y_LOG_LEVEL_ERROR, "r_jws_sign_hmac - Error key invalid, 'alg' invalid");
  }

  if (key != NULL && sig != NULL) {
    data = (unsigned char *)msprintf("%s.%s", jws->header_b64url, jws->payload_b64url);
    if (!gnutls_hmac_fast(alg, key, key_len, data, o_strlen((const char *)data), sig)) {
      if (o_base64url_encode_alloc(sig, sig_len, &dat_sig)) {
        to_return = (unsigned char*)o_strndup((const char *)dat_sig.data, dat_sig.size);
        o_free(dat_sig.data);
      } else {
        y_log_message(Y_LOG_LEVEL_ERROR, "r_jws_sign_hmac - Error o_base64url_encode sig_b64");
      }
    } else {
      y_log_message(Y_LOG_LEVEL_ERROR, "r_jws_sign_hmac - Error gnutls_hmac_fast");
    }
  }

  o_free(data);
  o_free(sig);
  o_free(key);

  return to_return;
}

static unsigned char * r_jws_sign_rsa(jws_t * jws, jwk_t * jwk) {
  gnutls_privkey_t privkey = r_jwk_export_to_gnutls_privkey(jwk);
  gnutls_datum_t body_dat, sig_dat;
  unsigned char * to_return = NULL;
  int alg = GNUTLS_DIG_NULL, res;
  unsigned int flag = 0;
  struct _o_datum dat_sig = {0, NULL};

  switch (jws->alg) {
    case R_JWA_ALG_RS256:
      alg = GNUTLS_DIG_SHA256;
      break;
    case R_JWA_ALG_RS384:
      alg = GNUTLS_DIG_SHA384;
      break;
    case R_JWA_ALG_RS512:
      alg = GNUTLS_DIG_SHA512;
      break;
/* RSA-PSS signature is available with GnuTLS >= 3.6 */
#if GNUTLS_VERSION_NUMBER >= 0x030600
    case R_JWA_ALG_PS256:
      alg = GNUTLS_SIGN_RSA_PSS_SHA256;
      flag = GNUTLS_PRIVKEY_SIGN_FLAG_RSA_PSS;
      break;
    case R_JWA_ALG_PS384:
      alg = GNUTLS_SIGN_RSA_PSS_SHA384;
      flag = GNUTLS_PRIVKEY_SIGN_FLAG_RSA_PSS;
      break;
    case R_JWA_ALG_PS512:
      alg = GNUTLS_SIGN_RSA_PSS_SHA512;
      flag = GNUTLS_PRIVKEY_SIGN_FLAG_RSA_PSS;
      break;
#endif
    default:
      break;
  }

  if (privkey != NULL && GNUTLS_PK_RSA == gnutls_privkey_get_pk_algorithm(privkey, NULL)) {
    body_dat.data = (unsigned char *)msprintf("%s.%s", jws->header_b64url, jws->payload_b64url);
    body_dat.size = (unsigned int)o_strlen((const char *)body_dat.data);

    if (!(res =
#if GNUTLS_VERSION_NUMBER >= 0x030600
                 gnutls_privkey_sign_data2
#else
                 gnutls_privkey_sign_data
#endif
                                           (privkey, alg, flag, &body_dat, &sig_dat))) {
      if (o_base64url_encode_alloc(sig_dat.data, sig_dat.size, &dat_sig)) {
        to_return = (unsigned char*)o_strndup((const char *)dat_sig.data, dat_sig.size);
        o_free(dat_sig.data);
      } else {
        y_log_message(Y_LOG_LEVEL_ERROR, "r_jws_sign_rsa - Error o_base64url_encode for to_return");
        o_free(to_return);
        to_return = NULL;
      }
      gnutls_free(sig_dat.data);
    } else {
      y_log_message(Y_LOG_LEVEL_ERROR, "r_jws_sign_rsa - Error gnutls_privkey_sign_data2, res %d", res);
    }
    o_free(body_dat.data);
  } else {
    y_log_message(Y_LOG_LEVEL_ERROR, "r_jws_sign_rsa - Error extracting privkey");
  }
  gnutls_privkey_deinit(privkey);
  return to_return;
}

static unsigned char * r_jws_sign_ecdsa(jws_t * jws, jwk_t * jwk) {
#if GNUTLS_VERSION_NUMBER >= 0x030600
  gnutls_privkey_t privkey = r_jwk_export_to_gnutls_privkey(jwk);
  gnutls_datum_t body_dat, sig_dat, r, s;
  unsigned char * binary_sig = NULL, * to_return = NULL;
  int alg = GNUTLS_DIG_NULL, res;
  unsigned int adj = 0;
  unsigned int r_padding = 0, s_padding = 0, r_out_padding = 0, s_out_padding = 0;
  size_t sig_size;
  struct _o_datum dat_sig = {0, NULL};

  if (jws->alg == R_JWA_ALG_ES256) {
    alg = GNUTLS_DIG_SHA256;
    adj = 32;
  } else if (jws->alg == R_JWA_ALG_ES384) {
    alg = GNUTLS_DIG_SHA384;
    adj = 48;
  } else if (jws->alg == R_JWA_ALG_ES512) {
    alg = GNUTLS_DIG_SHA512;
    adj = 66;
  }

  if (privkey != NULL && GNUTLS_PK_EC == gnutls_privkey_get_pk_algorithm(privkey, NULL)) {
    body_dat.data = (unsigned char *)msprintf("%s.%s", jws->header_b64url, jws->payload_b64url);
    body_dat.size = (unsigned int)o_strlen((const char *)body_dat.data);

    if (!(res = gnutls_privkey_sign_data(privkey, alg, 0, &body_dat, &sig_dat))) {
      if (!gnutls_decode_rs_value(&sig_dat, &r, &s)) {
        if (r.size > adj) {
          r_padding = r.size - adj;
        } else if (r.size < adj) {
          r_out_padding = adj - r.size;
        }

        if (s.size > adj) {
          s_padding = s.size - adj;
        } else if (s.size < adj) {
          s_out_padding = adj - s.size;
        }

        sig_size = adj << 1;

        if ((binary_sig = o_malloc(sig_size)) != NULL) {
          memset(binary_sig, 0, sig_size);
          memcpy(binary_sig + r_out_padding, r.data + r_padding, r.size - r_padding);
          memcpy(binary_sig + (r.size - r_padding + r_out_padding) + s_out_padding, s.data + s_padding, (s.size - s_padding));
          if (o_base64url_encode_alloc(binary_sig, sig_size, &dat_sig)) {
            to_return = (unsigned char*)o_strndup((const char *)dat_sig.data, dat_sig.size);
            o_free(dat_sig.data);
          } else {
            y_log_message(Y_LOG_LEVEL_ERROR, "r_jws_sign_ecdsa - Error o_base64url_encode_alloc for dat_sig");
          }
          o_free(binary_sig);
        } else {
          y_log_message(Y_LOG_LEVEL_ERROR, "r_jws_sign_ecdsa - Error allocating resources for binary_sig");
        }
        gnutls_free(r.data);
        gnutls_free(s.data);
      } else {
        y_log_message(Y_LOG_LEVEL_ERROR, "r_jws_sign_ecdsa - Error gnutls_decode_rs_value");
      }
      gnutls_free(sig_dat.data);
    } else {
      y_log_message(Y_LOG_LEVEL_ERROR, "r_jws_sign_ecdsa - Error gnutls_privkey_sign_data: %d", res);
    }
    o_free(body_dat.data);
  } else {
    y_log_message(Y_LOG_LEVEL_ERROR, "r_jws_sign_ecdsa - Error extracting privkey");
  }
  gnutls_privkey_deinit(privkey);
  return to_return;
#else
  (void)(jws);
  (void)(jwk);
  return NULL;
#endif
}

static unsigned char * r_jws_sign_eddsa(jws_t * jws, jwk_t * jwk) {
#if GNUTLS_VERSION_NUMBER >= 0x030600
  gnutls_privkey_t privkey = r_jwk_export_to_gnutls_privkey(jwk);
  gnutls_datum_t body_dat, sig_dat;
  unsigned char * to_return = NULL;
  int res;
  struct _o_datum dat_sig = {0, NULL};

  if (privkey != NULL && GNUTLS_PK_EDDSA_ED25519 == gnutls_privkey_get_pk_algorithm(privkey, NULL)) {
    body_dat.data = (unsigned char *)msprintf("%s.%s", jws->header_b64url, jws->payload_b64url);
    body_dat.size = (unsigned int)o_strlen((const char *)body_dat.data);

    if (!(res = gnutls_privkey_sign_data(privkey, GNUTLS_DIG_SHA512, 0, &body_dat, &sig_dat))) {
      if (o_base64url_encode_alloc(sig_dat.data, sig_dat.size, &dat_sig)) {
        to_return = (unsigned char*)o_strndup((const char *)dat_sig.data, dat_sig.size);
        o_free(dat_sig.data);
      } else {
        y_log_message(Y_LOG_LEVEL_ERROR, "r_jws_sign_eddsa - Error o_base64url_encode_alloc for dat_sig");
      }
      gnutls_free(sig_dat.data);
    } else {
      y_log_message(Y_LOG_LEVEL_ERROR, "r_jws_sign_eddsa - Error gnutls_privkey_sign_data: %d", res);
    }
    o_free(body_dat.data);
  } else {
    y_log_message(Y_LOG_LEVEL_ERROR, "r_jws_sign_eddsa - Error extracting privkey");
  }
  gnutls_privkey_deinit(privkey);
  return to_return;
#else
  (void)(jws);
  (void)(jwk);
  return NULL;
#endif
}

#if 0
static unsigned char * r_jws_sign_es256k(jws_t * jws, jwk_t * jwk) {
#if GNUTLS_VERSION_NUMBER >= 0x030600
  gnutls_privkey_t privkey = r_jwk_export_to_gnutls_privkey(jwk);
  gnutls_datum_t body_dat, sig_dat;
  unsigned char * to_return = NULL;
  int res;
  struct _o_datum dat_sig = {0, NULL};

  if (privkey != NULL && GNUTLS_PK_EC == gnutls_privkey_get_pk_algorithm(privkey, NULL)) {
    body_dat.data = (unsigned char *)msprintf("%s.%s", jws->header_b64url, jws->payload_b64url);
    body_dat.size = o_strlen((const char *)body_dat.data);

    if (!(res = gnutls_privkey_sign_data(privkey, GNUTLS_DIG_SHA256, 0, &body_dat, &sig_dat))) {
      if (o_base64url_encode_alloc(sig_dat.data, sig_dat.size, &dat_sig)) {
        to_return = (unsigned char*)o_strndup((const char *)dat_sig.data, dat_sig.size);
        o_free(dat_sig.data);
      } else {
        y_log_message(Y_LOG_LEVEL_ERROR, "r_jws_sign_es256k - Error o_base64url_encode for dat_sig");
      }
      gnutls_free(sig_dat.data);
    } else {
      y_log_message(Y_LOG_LEVEL_ERROR, "r_jws_sign_es256k - Error gnutls_privkey_sign_data: %d", res);
    }
    o_free(body_dat.data);
  } else {
    y_log_message(Y_LOG_LEVEL_ERROR, "r_jws_sign_es256k - Error extracting privkey");
  }
  gnutls_privkey_deinit(privkey);
  return to_return;
#else
  (void)(jws);
  (void)(jwk);
  return NULL;
#endif
}
#endif

static int r_jws_verify_sig_hmac(jws_t * jws, jwk_t * jwk) {
  unsigned char * sig = r_jws_sign_hmac(jws, jwk);
  int ret;

  if (sig != NULL && 0 == o_strcmp((const char *)jws->signature_b64url, (const char *)sig)) {
    ret = RHN_OK;
  } else {
    ret = RHN_ERROR_INVALID;
  }
  o_free(sig);
  return ret;
}

static int r_jws_verify_sig_rsa(jws_t * jws, jwk_t * jwk, int x5u_flags) {
  int alg = GNUTLS_DIG_NULL, ret = RHN_OK;
  unsigned int flag = 0;
  gnutls_datum_t sig_dat = {NULL, 0}, data;
  gnutls_pubkey_t pubkey = r_jwk_export_to_gnutls_pubkey(jwk, x5u_flags);
  struct _o_datum dat_sig = {0, NULL};

  data.data = (unsigned char *)msprintf("%s.%s", jws->header_b64url, jws->payload_b64url);
  data.size = (unsigned int)o_strlen((const char *)data.data);

  switch (jws->alg) {
    case R_JWA_ALG_RS256:
      alg = GNUTLS_DIG_SHA256;
      break;
    case R_JWA_ALG_RS384:
      alg = GNUTLS_DIG_SHA384;
      break;
    case R_JWA_ALG_RS512:
      alg = GNUTLS_DIG_SHA512;
      break;
#if GNUTLS_VERSION_NUMBER >= 0x030600
    case R_JWA_ALG_PS256:
      alg = GNUTLS_SIGN_RSA_PSS_SHA256;
      flag = GNUTLS_PRIVKEY_SIGN_FLAG_RSA_PSS;
      break;
    case R_JWA_ALG_PS384:
      alg = GNUTLS_SIGN_RSA_PSS_SHA384;
      flag = GNUTLS_PRIVKEY_SIGN_FLAG_RSA_PSS;
      break;
    case R_JWA_ALG_PS512:
      alg = GNUTLS_SIGN_RSA_PSS_SHA512;
      flag = GNUTLS_PRIVKEY_SIGN_FLAG_RSA_PSS;
      break;
#endif
    default:
      break;
  }

  if (pubkey != NULL && GNUTLS_PK_RSA == gnutls_pubkey_get_pk_algorithm(pubkey, NULL)) {
    if (!o_strnullempty((const char *)jws->signature_b64url)) {
      if (o_base64url_decode_alloc(jws->signature_b64url, o_strlen((const char *)jws->signature_b64url), &dat_sig)) {
        sig_dat.data = dat_sig.data;
        sig_dat.size = (unsigned int)dat_sig.size;
        if (gnutls_pubkey_verify_data2(pubkey, alg, flag, &data, &sig_dat)) {
          y_log_message(Y_LOG_LEVEL_ERROR, "r_jws_verify_sig_rsa - Error invalid signature");
          ret = RHN_ERROR_INVALID;
        }
        o_free(dat_sig.data);
      } else {
        y_log_message(Y_LOG_LEVEL_ERROR, "r_jws_verify_sig_rsa - Error o_base64url_decode_alloc for dat_sig");
        ret = RHN_ERROR;
      }
    } else {
      y_log_message(Y_LOG_LEVEL_ERROR, "r_jws_verify_sig_rsa - Error signature empty");
      ret = RHN_ERROR_INVALID;
    }
  } else {
    y_log_message(Y_LOG_LEVEL_ERROR, "r_jws_verify_sig_rsa - Invalid public key");
    ret = RHN_ERROR_PARAM;
  }
  o_free(data.data);
  gnutls_pubkey_deinit(pubkey);
  return ret;
}

static int r_jws_verify_sig_ecdsa(jws_t * jws, jwk_t * jwk, int x5u_flags) {
#if GNUTLS_VERSION_NUMBER >= 0x030600
  int alg = 0, ret = RHN_OK;
  gnutls_datum_t sig_dat = {NULL, 0}, r, s, data;
  gnutls_pubkey_t pubkey = r_jwk_export_to_gnutls_pubkey(jwk, x5u_flags);
  struct _o_datum dat_sig = {0, NULL};

  data.data = (unsigned char *)msprintf("%s.%s", jws->header_b64url, jws->payload_b64url);
  data.size = (unsigned int)o_strlen((const char *)data.data);

  switch (jws->alg) {
    case R_JWA_ALG_ES256:
      alg = GNUTLS_SIGN_ECDSA_SHA256;
      break;
    case R_JWA_ALG_ES384:
      alg = GNUTLS_SIGN_ECDSA_SHA384;
      break;
    case R_JWA_ALG_ES512:
      alg = GNUTLS_SIGN_ECDSA_SHA512;
      break;
    default:
      break;
  }

  if (pubkey != NULL && GNUTLS_PK_EC == gnutls_pubkey_get_pk_algorithm(pubkey, NULL)) {
    if (!o_strnullempty((const char *)jws->signature_b64url)) {
      if (o_base64url_decode_alloc(jws->signature_b64url, o_strlen((const char *)jws->signature_b64url), &dat_sig)) {
        if (dat_sig.size == 64) {
          r.size = 32;
          r.data = dat_sig.data;
          s.size = 32;
          s.data = dat_sig.data + 32;
        } else if (dat_sig.size == 96) {
          r.size = 48;
          r.data = dat_sig.data;
          s.size = 48;
          s.data = dat_sig.data + 48;
        } else if (dat_sig.size == 132) {
          r.size = 66;
          r.data = dat_sig.data;
          s.size = 66;
          s.data = dat_sig.data + 66;
        } else {
          y_log_message(Y_LOG_LEVEL_ERROR, "r_jws_verify_sig_ecdsa - Error invalid signature length");
          ret = RHN_ERROR_INVALID;
        }

        if (ret == RHN_OK) {
          if (!gnutls_encode_rs_value(&sig_dat, &r, &s)) {
            if (gnutls_pubkey_verify_data2(pubkey, alg, 0, &data, &sig_dat)) {
              y_log_message(Y_LOG_LEVEL_ERROR, "r_jws_verify_sig_ecdsa - Error invalid signature");
              ret = RHN_ERROR_INVALID;
            }
            if (sig_dat.data != NULL) {
              gnutls_free(sig_dat.data);
            }
          } else {
            y_log_message(Y_LOG_LEVEL_ERROR, "r_jws_verify_sig_ecdsa - Error gnutls_encode_rs_value");
            ret = RHN_ERROR;
          }
        }
        o_free(dat_sig.data);
      } else {
        y_log_message(Y_LOG_LEVEL_ERROR, "r_jws_verify_sig_ecdsa - Error o_base64url_decode_alloc for dat_sig");
        ret = RHN_ERROR;
      }
    } else {
      y_log_message(Y_LOG_LEVEL_ERROR, "r_jws_verify_sig_ecdsa - Error signature empty");
      ret = RHN_ERROR_INVALID;
    }
  } else {
    y_log_message(Y_LOG_LEVEL_ERROR, "r_jws_verify_sig_ecdsa - Invalid public key");
    ret = RHN_ERROR_PARAM;
  }
  o_free(data.data);
  gnutls_pubkey_deinit(pubkey);
  return ret;
#else
  (void)(jws);
  (void)(jwk);
  (void)(x5u_flags);
  return RHN_ERROR_INVALID;
#endif
}

static int r_jws_verify_sig_eddsa(jws_t * jws, jwk_t * jwk, int x5u_flags) {
#if GNUTLS_VERSION_NUMBER >= 0x030600
  int ret = RHN_OK;
  gnutls_datum_t sig_dat = {NULL, 0}, data;
  gnutls_pubkey_t pubkey = r_jwk_export_to_gnutls_pubkey(jwk, x5u_flags);
  struct _o_datum dat_sig = {0, NULL};

  data.data = (unsigned char *)msprintf("%s.%s", jws->header_b64url, jws->payload_b64url);
  data.size = (unsigned int)o_strlen((const char *)data.data);

  if (pubkey != NULL && GNUTLS_PK_EDDSA_ED25519 == gnutls_pubkey_get_pk_algorithm(pubkey, NULL)) {
    if (!o_strnullempty((const char *)jws->signature_b64url)) {
      if (o_base64url_decode_alloc(jws->signature_b64url, o_strlen((const char *)jws->signature_b64url), &dat_sig)) {
        sig_dat.data = dat_sig.data;
        sig_dat.size = (unsigned int)dat_sig.size;
        if (gnutls_pubkey_verify_data2(pubkey, GNUTLS_SIGN_EDDSA_ED25519, 0, &data, &sig_dat)) {
          y_log_message(Y_LOG_LEVEL_ERROR, "r_jws_verify_sig_eddsa - Error invalid signature");
          ret = RHN_ERROR_INVALID;
        }
        o_free(dat_sig.data);
      } else {
        y_log_message(Y_LOG_LEVEL_ERROR, "r_jws_verify_sig_eddsa - Error o_base64url_decode for dat_sig");
        ret = RHN_ERROR;
      }
    } else {
      y_log_message(Y_LOG_LEVEL_ERROR, "r_jws_verify_sig_eddsa - Error signature empty");
      ret = RHN_ERROR_INVALID;
    }
  } else {
    y_log_message(Y_LOG_LEVEL_ERROR, "r_jws_verify_sig_eddsa - Invalid public key");
    ret = RHN_ERROR_PARAM;
  }
  o_free(data.data);
  gnutls_pubkey_deinit(pubkey);
  return ret;
#else
  (void)(jws);
  (void)(jwk);
  (void)(x5u_flags);
  return RHN_ERROR_INVALID;
#endif
}

#if 0
static int r_jws_verify_sig_es256k(jws_t * jws, jwk_t * jwk, int x5u_flags) {
#if GNUTLS_VERSION_NUMBER >= 0x030600
  int ret = RHN_OK;
  gnutls_datum_t sig_dat = {NULL, 0}, data;
  gnutls_pubkey_t pubkey = r_jwk_export_to_gnutls_pubkey(jwk, x5u_flags);
  struct _o_datum dat_sig = {0, NULL};

  data.data = (unsigned char *)msprintf("%s.%s", jws->header_b64url, jws->payload_b64url);
  data.size = o_strlen((const char *)data.data);

  if (pubkey != NULL && GNUTLS_PK_EC == gnutls_pubkey_get_pk_algorithm(pubkey, NULL)) {
    if (!o_strnullempty((const char *)jws->signature_b64url)) {
      if (o_base64url_decode_alloc(jws->signature_b64url, o_strlen((const char *)jws->signature_b64url), &dat_sig)) {
        sig_dat.data = dat_sig.data;
        sig_dat.size = dat_sig.size;
        if (gnutls_pubkey_verify_data2(pubkey, GNUTLS_SIGN_ECDSA_SHA256, 0, &data, &sig_dat)) {
          y_log_message(Y_LOG_LEVEL_ERROR, "r_jws_verify_sig_es256k - Error invalid signature");
          ret = RHN_ERROR_INVALID;
        }
        o_free(dat_sig.data);
      } else {
        y_log_message(Y_LOG_LEVEL_ERROR, "r_jws_verify_sig_es256k - Error o_base64url_decode_alloc for dat_sig");
        ret = RHN_ERROR;
      }
    } else {
      y_log_message(Y_LOG_LEVEL_ERROR, "r_jws_verify_sig_es256k - Error signature empty");
      ret = RHN_ERROR_INVALID;
    }
  } else {
    y_log_message(Y_LOG_LEVEL_ERROR, "r_jws_verify_sig_es256k - Invalid public key");
    ret = RHN_ERROR_PARAM;
  }
  o_free(data.data);
  gnutls_pubkey_deinit(pubkey);
  return ret;
#else
  (void)(jws);
  (void)(jwk);
  (void)(x5u_flags);
  return RHN_ERROR_INVALID;
#endif
}
#endif

static int _r_verify_signature(jws_t * jws, jwk_t * jwk, jwa_alg alg, int x5u_flags) {
  int ret;

  switch (alg) {
    case R_JWA_ALG_HS256:
    case R_JWA_ALG_HS384:
    case R_JWA_ALG_HS512:
      if (r_jwk_key_type(jwk, NULL, x5u_flags) & R_KEY_TYPE_HMAC) {
        ret = r_jws_verify_sig_hmac(jws, jwk);
      } else {
        ret = RHN_ERROR_INVALID;
      }
      break;
    case R_JWA_ALG_RS256:
    case R_JWA_ALG_RS384:
    case R_JWA_ALG_RS512:
    case R_JWA_ALG_PS256:
    case R_JWA_ALG_PS384:
    case R_JWA_ALG_PS512:
      if (r_jwk_key_type(jwk, NULL, x5u_flags) & R_KEY_TYPE_RSA) {
        ret = r_jws_verify_sig_rsa(jws, jwk, x5u_flags);
      } else {
        ret = RHN_ERROR_INVALID;
      }
      break;
    case R_JWA_ALG_ES256:
    case R_JWA_ALG_ES384:
    case R_JWA_ALG_ES512:
      if (r_jwk_key_type(jwk, NULL, x5u_flags) & R_KEY_TYPE_EC) {
        ret = r_jws_verify_sig_ecdsa(jws, jwk, x5u_flags);
      } else {
        ret = RHN_ERROR_INVALID;
      }
      break;
    case R_JWA_ALG_EDDSA:
      if (r_jwk_key_type(jwk, NULL, x5u_flags) & R_KEY_TYPE_EDDSA) {
        ret = r_jws_verify_sig_eddsa(jws, jwk, x5u_flags);
      } else {
        ret = RHN_ERROR_INVALID;
      }
      break;
#if 0
    case R_JWA_ALG_ES256K:
      if (r_jwk_key_type(jwk, NULL, x5u_flags) & R_KEY_TYPE_EC) {
        ret = r_jws_verify_sig_es256k(jws, jwk, x5u_flags);
      } else {
        ret = RHN_ERROR_INVALID;
      }
      break;
#endif
    default:
      ret = RHN_ERROR_INVALID;
      break;
  }
  return ret;
}

static unsigned char * _r_generate_signature(jws_t * jws, jwk_t * jwk, jwa_alg alg, int x5u_flags) {
  unsigned char * str_ret = NULL;
  int res;

  if (jws != NULL && (jwk != NULL || alg == R_JWA_ALG_NONE)) {
    switch (alg) {
      case R_JWA_ALG_HS256:
      case R_JWA_ALG_HS384:
      case R_JWA_ALG_HS512:
        if (r_jwk_key_type(jwk, NULL, x5u_flags) & R_KEY_TYPE_HMAC) {
          str_ret = r_jws_sign_hmac(jws, jwk);
        }
        break;
      case R_JWA_ALG_RS256:
      case R_JWA_ALG_RS384:
      case R_JWA_ALG_RS512:
      case R_JWA_ALG_PS256:
      case R_JWA_ALG_PS384:
      case R_JWA_ALG_PS512:
        res = r_jwk_key_type(jwk, NULL, x5u_flags);
        if (res & R_KEY_TYPE_RSA && res &R_KEY_TYPE_PRIVATE) {
          str_ret = r_jws_sign_rsa(jws, jwk);
        }
        break;
      case R_JWA_ALG_ES256:
      case R_JWA_ALG_ES384:
      case R_JWA_ALG_ES512:
        res = r_jwk_key_type(jwk, NULL, x5u_flags);
        if (res & R_KEY_TYPE_EC && res & R_KEY_TYPE_PRIVATE) {
          str_ret = r_jws_sign_ecdsa(jws, jwk);
        }
        break;
      case R_JWA_ALG_EDDSA:
        res = r_jwk_key_type(jwk, NULL, x5u_flags);
        if (res & R_KEY_TYPE_EDDSA && res & R_KEY_TYPE_PRIVATE) {
          str_ret = r_jws_sign_eddsa(jws, jwk);
        }
        break;
      case R_JWA_ALG_NONE:
        str_ret = (unsigned char *)o_strdup("");
        break;
#if 0
      case R_JWA_ALG_ES256K:
        res = r_jwk_key_type(jwk, NULL, x5u_flags);
        if (res & R_KEY_TYPE_EC && res & R_KEY_TYPE_PRIVATE) {
          str_ret = r_jws_sign_es256k(jws, jwk);
        }
        break;
#endif
      default:
        y_log_message(Y_LOG_LEVEL_ERROR, "_r_generate_signature - Unsupported algorithm");
        break;
    }
  } else {
    y_log_message(Y_LOG_LEVEL_ERROR, "_r_generate_signature - Error input parameters");
  }
  return str_ret;
}

int r_jws_init(jws_t ** jws) {
  int ret;

  if (jws != NULL) {
    if ((*jws = o_malloc(sizeof(jws_t))) != NULL) {
      if (((*jws)->j_header = json_object()) != NULL) {
        if (r_jwks_init(&(*jws)->jwks_pubkey) == RHN_OK) {
          if (r_jwks_init(&(*jws)->jwks_privkey) == RHN_OK) {
            (*jws)->alg = R_JWA_ALG_UNKNOWN;
            (*jws)->header_b64url = NULL;
            (*jws)->payload_b64url = NULL;
            (*jws)->signature_b64url = NULL;
            (*jws)->payload = NULL;
            (*jws)->payload_len = 0;
            (*jws)->j_json_serialization = NULL;
            (*jws)->token_mode = R_JSON_MODE_COMPACT;
            ret = RHN_OK;
          } else {
            y_log_message(Y_LOG_LEVEL_ERROR, "r_jws_init - Error allocating resources for jwks_privkey");
            ret = RHN_ERROR_MEMORY;
          }
        } else {
          y_log_message(Y_LOG_LEVEL_ERROR, "r_jws_init - Error allocating resources for jwks_pubkey");
          ret = RHN_ERROR_MEMORY;
        }
      } else {
        y_log_message(Y_LOG_LEVEL_ERROR, "r_jws_init - Error allocating resources for j_header");
        ret = RHN_ERROR_MEMORY;
      }
    } else {
      y_log_message(Y_LOG_LEVEL_ERROR, "r_jws_init - Error allocating resources for jws");
      ret = RHN_ERROR_MEMORY;
    }
  } else {
    ret = RHN_ERROR_PARAM;
  }
  if (ret != RHN_OK && jws != NULL) {
    r_jws_free(*jws);
    *jws = NULL;
  }
  return ret;
}

void r_jws_free(jws_t * jws) {
  if (jws != NULL) {
    r_jwks_free(jws->jwks_privkey);
    r_jwks_free(jws->jwks_pubkey);
    o_free(jws->header_b64url);
    o_free(jws->payload_b64url);
    o_free(jws->signature_b64url);
    json_decref(jws->j_header);
    o_free(jws->payload);
    json_decref(jws->j_json_serialization);
    o_free(jws);
  }
}

jws_t * r_jws_copy(jws_t * jws) {
  jws_t * jws_copy = NULL;
  if (jws != NULL) {
    if (r_jws_init(&jws_copy) == RHN_OK) {
      if (r_jws_set_payload(jws_copy, jws->payload, jws->payload_len) == RHN_OK) {
        jws_copy->header_b64url = (unsigned char *)o_strdup((const char *)jws->header_b64url);
        jws_copy->payload_b64url = (unsigned char *)o_strdup((const char *)jws->payload_b64url);
        jws_copy->signature_b64url = (unsigned char *)o_strdup((const char *)jws->signature_b64url);
        jws_copy->alg = jws->alg;
        r_jwks_free(jws_copy->jwks_privkey);
        jws_copy->jwks_privkey = r_jwks_copy(jws->jwks_privkey);
        r_jwks_free(jws_copy->jwks_pubkey);
        jws_copy->jwks_pubkey = r_jwks_copy(jws->jwks_pubkey);
        json_decref(jws_copy->j_header);
        jws_copy->j_header = json_deep_copy(jws->j_header);
        jws_copy->j_json_serialization = json_deep_copy(jws->j_json_serialization);
      } else {
        y_log_message(Y_LOG_LEVEL_ERROR, "r_jws_copy - Error allocating resources for jws_copy->payload");
        r_jws_free(jws_copy);
        jws_copy = NULL;
      }
    } else {
      y_log_message(Y_LOG_LEVEL_ERROR, "r_jws_copy - Error r_jws_init");
    }
  }
  return jws_copy;
}

int r_jws_set_payload(jws_t * jws, const unsigned char * payload, size_t payload_len) {
  int ret;

  if (jws != NULL) {
    o_free(jws->payload);
    if (payload != NULL && payload_len) {
      if ((jws->payload = o_malloc(payload_len)) != NULL) {
        memcpy(jws->payload, payload, payload_len);
        jws->payload_len = payload_len;
        ret = RHN_OK;
      } else {
        y_log_message(Y_LOG_LEVEL_ERROR, "r_jws_set_payload - Error allocating resources for payload");
        ret = RHN_ERROR_MEMORY;
      }
    } else {
      jws->payload = NULL;
      jws->payload_len = 0;
      ret = RHN_OK;
    }
  } else {
    ret = RHN_ERROR_PARAM;
  }
  return ret;
}

const unsigned char * r_jws_get_payload(jws_t * jws, size_t * payload_len) {
  if (jws != NULL) {
    if (payload_len != NULL) {
      *payload_len = jws->payload_len;
    }
    return jws->payload;
  }
  return NULL;
}

int r_jws_set_alg(jws_t * jws, jwa_alg alg) {
  int ret = RHN_OK;

  if (jws != NULL) {
    switch (alg) {
      case R_JWA_ALG_NONE:
        json_object_set_new(jws->j_header, "alg", json_string("none"));
        break;
      case R_JWA_ALG_HS256:
        json_object_set_new(jws->j_header, "alg", json_string("HS256"));
        break;
      case R_JWA_ALG_HS384:
        json_object_set_new(jws->j_header, "alg", json_string("HS384"));
        break;
      case R_JWA_ALG_HS512:
        json_object_set_new(jws->j_header, "alg", json_string("HS512"));
        break;
      case R_JWA_ALG_RS256:
        json_object_set_new(jws->j_header, "alg", json_string("RS256"));
        break;
      case R_JWA_ALG_RS384:
        json_object_set_new(jws->j_header, "alg", json_string("RS384"));
        break;
      case R_JWA_ALG_RS512:
        json_object_set_new(jws->j_header, "alg", json_string("RS512"));
        break;
      case R_JWA_ALG_ES256:
        json_object_set_new(jws->j_header, "alg", json_string("ES256"));
        break;
      case R_JWA_ALG_ES384:
        json_object_set_new(jws->j_header, "alg", json_string("ES384"));
        break;
      case R_JWA_ALG_ES512:
        json_object_set_new(jws->j_header, "alg", json_string("ES512"));
        break;
      case R_JWA_ALG_PS256:
        json_object_set_new(jws->j_header, "alg", json_string("PS256"));
        break;
      case R_JWA_ALG_PS384:
        json_object_set_new(jws->j_header, "alg", json_string("PS384"));
        break;
      case R_JWA_ALG_PS512:
        json_object_set_new(jws->j_header, "alg", json_string("PS512"));
        break;
      case R_JWA_ALG_EDDSA:
        json_object_set_new(jws->j_header, "alg", json_string("EdDSA"));
        break;
      case R_JWA_ALG_ES256K:
        json_object_set_new(jws->j_header, "alg", json_string("ES256K"));
        break;
      default:
        ret = RHN_ERROR_PARAM;
        break;
    }
    if (ret == RHN_OK) {
      jws->alg = alg;
    }
  } else {
    ret = RHN_ERROR_PARAM;
  }
  return ret;
}

jwa_alg r_jws_get_alg(jws_t * jws) {
  if (jws != NULL) {
    return jws->alg;
  } else {
    return R_JWA_ALG_UNKNOWN;
  }
}

const char * r_jws_get_kid(jws_t * jws) {
  const char * kid = r_jws_get_header_str_value(jws, "kid");
  if (!!o_strnullempty(kid)) {
    if (jws != NULL && jws->token_mode == R_JSON_MODE_FLATTENED) {
      kid = json_string_value(json_object_get(json_object_get(jws->j_json_serialization, "header"), "kid"));
    }
  }
  return kid;
}

int r_jws_set_header_str_value(jws_t * jws, const char * key, const char * str_value) {
  int ret;

  if (jws != NULL) {
    if ((ret = _r_json_set_str_value(jws->j_header, key, str_value)) == RHN_OK) {
      o_free(jws->header_b64url);
      jws->header_b64url = NULL;
    }
    return ret;
  } else {
    return RHN_ERROR_PARAM;
  }
}

int r_jws_set_header_int_value(jws_t * jws, const char * key, rhn_int_t i_value) {
  int ret;

  if (jws != NULL) {
    if ((ret = _r_json_set_int_value(jws->j_header, key, i_value)) == RHN_OK) {
      o_free(jws->header_b64url);
      jws->header_b64url = NULL;
    }
    return ret;
  } else {
    return RHN_ERROR_PARAM;
  }
}

int r_jws_set_header_json_t_value(jws_t * jws, const char * key, json_t * j_value) {
  int ret;

  if (jws != NULL) {
    if ((ret = _r_json_set_json_t_value(jws->j_header, key, j_value)) == RHN_OK) {
      o_free(jws->header_b64url);
      jws->header_b64url = NULL;
    }
    return ret;
  } else {
    ret = RHN_ERROR_PARAM;
  }
  return ret;
}

const char * r_jws_get_header_str_value(jws_t * jws, const char * key) {
  if (jws != NULL) {
    return _r_json_get_str_value(jws->j_header, key);
  }
  return NULL;
}

rhn_int_t r_jws_get_header_int_value(jws_t * jws, const char * key) {
  if (jws != NULL) {
    return _r_json_get_int_value(jws->j_header, key);
  }
  return 0;
}

json_t * r_jws_get_header_json_t_value(jws_t * jws, const char * key) {
  if (jws != NULL) {
    return _r_json_get_json_t_value(jws->j_header, key);
  }
  return NULL;
}

json_t * r_jws_get_full_header_json_t(jws_t * jws) {
  if (jws != NULL) {
    return _r_json_get_full_json_t(jws->j_header);
  }
  return NULL;
}

char * r_jws_get_full_header_str(jws_t * jws) {
  char * to_return = NULL;
  if (jws != NULL) {
    to_return = json_dumps(jws->j_header, JSON_COMPACT);
  }
  return to_return;
}

int r_jws_add_keys(jws_t * jws, jwk_t * jwk_privkey, jwk_t * jwk_pubkey) {
  int ret = RHN_OK;
  jwa_alg alg;

  if (jws != NULL && (jwk_privkey != NULL || jwk_pubkey != NULL)) {
    if (jwk_privkey != NULL) {
      if (r_jwks_append_jwk(jws->jwks_privkey, jwk_privkey) != RHN_OK) {
        y_log_message(Y_LOG_LEVEL_ERROR, "r_jws_add_keys - Error setting jwk_privkey");
        ret = RHN_ERROR;
      }
      if (jws->alg == R_JWA_ALG_UNKNOWN && (alg = r_str_to_jwa_alg(r_jwk_get_property_str(jwk_privkey, "alg"))) != R_JWA_ALG_NONE) {
        r_jws_set_alg(jws, alg);
      }
    }
    if (jwk_pubkey != NULL) {
      if (r_jwks_append_jwk(jws->jwks_pubkey, jwk_pubkey) != RHN_OK) {
        y_log_message(Y_LOG_LEVEL_ERROR, "r_jws_add_keys - Error setting jwk_pubkey");
        ret = RHN_ERROR;
      }
    }
  } else {
    ret = RHN_ERROR_PARAM;
  }
  return ret;
}

int r_jws_add_jwks(jws_t * jws, jwks_t * jwks_privkey, jwks_t * jwks_pubkey) {
  size_t i;
  int ret, res;
  jwk_t * jwk;

  if (jws != NULL && (jwks_privkey != NULL || jwks_pubkey != NULL)) {
    ret = RHN_OK;
    if (jwks_privkey != NULL) {
      for (i=0; ret==RHN_OK && i<r_jwks_size(jwks_privkey); i++) {
        jwk = r_jwks_get_at(jwks_privkey, i);
        if ((res = r_jws_add_keys(jws, jwk, NULL)) != RHN_OK) {
          y_log_message(Y_LOG_LEVEL_ERROR, "r_jws_add_jwks - Error r_jws_add_keys private key at %zu", i);
          ret = res;
        }
        r_jwk_free(jwk);
      }
    }
    if (jwks_pubkey != NULL) {
      for (i=0; ret==RHN_OK && i<r_jwks_size(jwks_pubkey); i++) {
        jwk = r_jwks_get_at(jwks_pubkey, i);
        if ((res = r_jws_add_keys(jws, NULL, jwk)) != RHN_OK) {
          y_log_message(Y_LOG_LEVEL_ERROR, "r_jws_add_jwks - Error r_jws_add_keys public key at %zu", i);
          ret = res;
        }
        r_jwk_free(jwk);
      }
    }
  } else {
    ret = RHN_ERROR_PARAM;
  }
  return ret;
}

int r_jws_add_keys_json_str(jws_t * jws, const char * privkey, const char * pubkey) {
  int ret = RHN_OK;
  jwa_alg alg;
  jwk_t * j_privkey = NULL, * j_pubkey = NULL;

  if (jws != NULL && (privkey != NULL || pubkey != NULL)) {
    if (privkey != NULL) {
      if (r_jwk_init(&j_privkey) == RHN_OK && r_jwk_import_from_json_str(j_privkey, privkey) == RHN_OK) {
        if (r_jwks_append_jwk(jws->jwks_privkey, j_privkey) != RHN_OK) {
          y_log_message(Y_LOG_LEVEL_ERROR, "r_jws_add_keys_json_str - Error setting privkey");
          ret = RHN_ERROR;
        }
        if (jws->alg == R_JWA_ALG_UNKNOWN && (alg = r_str_to_jwa_alg(r_jwk_get_property_str(j_privkey, "alg"))) != R_JWA_ALG_NONE) {
          r_jws_set_alg(jws, alg);
        }
      } else {
        y_log_message(Y_LOG_LEVEL_ERROR, "r_jws_add_keys_json_str - Error parsing privkey");
        ret = RHN_ERROR;
      }
      r_jwk_free(j_privkey);
    }
    if (pubkey != NULL) {
      if (r_jwk_init(&j_pubkey) == RHN_OK && r_jwk_import_from_json_str(j_pubkey, pubkey) == RHN_OK) {
        if (r_jwks_append_jwk(jws->jwks_pubkey, j_pubkey) != RHN_OK) {
          y_log_message(Y_LOG_LEVEL_ERROR, "r_jws_add_keys_json_str - Error setting pubkey");
          ret = RHN_ERROR;
        }
      } else {
        y_log_message(Y_LOG_LEVEL_ERROR, "r_jws_add_keys_json_str - Error parsing pubkey");
        ret = RHN_ERROR;
      }
      r_jwk_free(j_pubkey);
    }
  } else {
    ret = RHN_ERROR_PARAM;
  }
  return ret;
}

int r_jws_add_keys_json_t(jws_t * jws, json_t * privkey, json_t * pubkey) {
  int ret = RHN_OK;
  jwa_alg alg;
  jwk_t * j_privkey = NULL, * j_pubkey = NULL;

  if (jws != NULL && (privkey != NULL || pubkey != NULL)) {
    if (privkey != NULL) {
      if (r_jwk_init(&j_privkey) == RHN_OK && r_jwk_import_from_json_t(j_privkey, privkey) == RHN_OK) {
        if (r_jwks_append_jwk(jws->jwks_privkey, j_privkey) != RHN_OK) {
          y_log_message(Y_LOG_LEVEL_ERROR, "r_jws_add_keys_json_t - Error setting privkey");
          ret = RHN_ERROR;
        }
        if (jws->alg == R_JWA_ALG_UNKNOWN && (alg = r_str_to_jwa_alg(r_jwk_get_property_str(j_privkey, "alg"))) != R_JWA_ALG_NONE) {
          r_jws_set_alg(jws, alg);
        }
      } else {
        y_log_message(Y_LOG_LEVEL_ERROR, "r_jws_add_keys_json_t - Error parsing privkey");
        ret = RHN_ERROR;
      }
      r_jwk_free(j_privkey);
    }
    if (pubkey != NULL) {
      if (r_jwk_init(&j_pubkey) == RHN_OK && r_jwk_import_from_json_t(j_pubkey, pubkey) == RHN_OK) {
        if (r_jwks_append_jwk(jws->jwks_pubkey, j_pubkey) != RHN_OK) {
          y_log_message(Y_LOG_LEVEL_ERROR, "r_jws_add_keys_json_t - Error setting pubkey");
          ret = RHN_ERROR;
        }
      } else {
        y_log_message(Y_LOG_LEVEL_ERROR, "r_jws_add_keys_json_t - Error parsing pubkey");
        ret = RHN_ERROR;
      }
      r_jwk_free(j_pubkey);
    }
  } else {
    ret = RHN_ERROR_PARAM;
  }
  return ret;
}

int r_jws_add_keys_pem_der(jws_t * jws, int format, const unsigned char * privkey, size_t privkey_len, const unsigned char * pubkey, size_t pubkey_len) {
  int ret = RHN_OK;
  jwa_alg alg;
  jwk_t * j_privkey = NULL, * j_pubkey = NULL;

  if (jws != NULL && (privkey != NULL || pubkey != NULL)) {
    if (privkey != NULL) {
      if (r_jwk_init(&j_privkey) == RHN_OK && r_jwk_import_from_pem_der(j_privkey, R_X509_TYPE_PRIVKEY, format, privkey, privkey_len) == RHN_OK) {
        if (r_jwks_append_jwk(jws->jwks_privkey, j_privkey) != RHN_OK) {
          y_log_message(Y_LOG_LEVEL_ERROR, "r_jws_add_keys_pem_der - Error setting privkey");
          ret = RHN_ERROR;
        }
        if (jws->alg == R_JWA_ALG_UNKNOWN && (alg = r_str_to_jwa_alg(r_jwk_get_property_str(j_privkey, "alg"))) != R_JWA_ALG_NONE) {
          r_jws_set_alg(jws, alg);
        }
      } else {
        y_log_message(Y_LOG_LEVEL_ERROR, "r_jws_add_keys_pem_der - Error parsing privkey");
        ret = RHN_ERROR;
      }
      r_jwk_free(j_privkey);
    }
    if (pubkey != NULL) {
      if (r_jwk_init(&j_pubkey) == RHN_OK && r_jwk_import_from_pem_der(j_pubkey, R_X509_TYPE_PUBKEY, format, pubkey, pubkey_len) == RHN_OK) {
        if (r_jwks_append_jwk(jws->jwks_pubkey, j_pubkey) != RHN_OK) {
          y_log_message(Y_LOG_LEVEL_ERROR, "r_jws_add_keys_pem_der - Error setting pubkey");
          ret = RHN_ERROR;
        }
      } else {
        y_log_message(Y_LOG_LEVEL_ERROR, "r_jws_add_keys_pem_der - Error parsing pubkey");
        ret = RHN_ERROR;
      }
      r_jwk_free(j_pubkey);
    }
  } else {
    ret = RHN_ERROR_PARAM;
  }
  return ret;
}

int r_jws_add_keys_gnutls(jws_t * jws, gnutls_privkey_t privkey, gnutls_pubkey_t pubkey) {
  int ret = RHN_OK;
  jwa_alg alg;
  jwk_t * j_privkey = NULL, * j_pubkey = NULL;

  if (jws != NULL && (privkey != NULL || pubkey != NULL)) {
    if (privkey != NULL) {
      if (r_jwk_init(&j_privkey) == RHN_OK && r_jwk_import_from_gnutls_privkey(j_privkey, privkey) == RHN_OK) {
        if (r_jwks_append_jwk(jws->jwks_privkey, j_privkey) != RHN_OK) {
          y_log_message(Y_LOG_LEVEL_ERROR, "r_jws_add_keys_gnutls - Error setting privkey");
          ret = RHN_ERROR;
        }
        if (jws->alg == R_JWA_ALG_UNKNOWN && (alg = r_str_to_jwa_alg(r_jwk_get_property_str(j_privkey, "alg"))) != R_JWA_ALG_NONE) {
          r_jws_set_alg(jws, alg);
        }
      } else {
        y_log_message(Y_LOG_LEVEL_ERROR, "r_jws_add_keys_gnutls - Error parsing privkey");
        ret = RHN_ERROR;
      }
      r_jwk_free(j_privkey);
    }
    if (pubkey != NULL) {
      if (r_jwk_init(&j_pubkey) == RHN_OK && r_jwk_import_from_gnutls_pubkey(j_pubkey, pubkey) == RHN_OK) {
        if (r_jwks_append_jwk(jws->jwks_pubkey, j_pubkey) != RHN_OK) {
          y_log_message(Y_LOG_LEVEL_ERROR, "r_jws_add_keys_gnutls - Error setting pubkey");
          ret = RHN_ERROR;
        }
      } else {
        y_log_message(Y_LOG_LEVEL_ERROR, "r_jws_add_keys_gnutls - Error parsing pubkey");
        ret = RHN_ERROR;
      }
      r_jwk_free(j_pubkey);
    }
  } else {
    ret = RHN_ERROR_PARAM;
  }
  return ret;
}

int r_jws_add_key_symmetric(jws_t * jws, const unsigned char * key, size_t key_len) {
  int ret = RHN_OK;
  jwa_alg alg;
  jwk_t * j_key = NULL;

  if (jws != NULL && key != NULL && key_len) {
    if (r_jwk_init(&j_key) == RHN_OK && r_jwk_import_from_symmetric_key(j_key, key, key_len) == RHN_OK) {
      if (r_jwks_append_jwk(jws->jwks_privkey, j_key) != RHN_OK || r_jwks_append_jwk(jws->jwks_pubkey, j_key) != RHN_OK) {
        y_log_message(Y_LOG_LEVEL_ERROR, "r_jws_add_sign_key_symmetric - Error setting key");
        ret = RHN_ERROR;
      }
      if (jws->alg == R_JWA_ALG_UNKNOWN && (alg = r_str_to_jwa_alg(r_jwk_get_property_str(j_key, "alg"))) != R_JWA_ALG_NONE) {
        r_jws_set_alg(jws, alg);
      }
    } else {
      y_log_message(Y_LOG_LEVEL_ERROR, "r_jws_add_sign_key_symmetric - Error parsing key");
      ret = RHN_ERROR;
    }
    r_jwk_free(j_key);
  } else {
    ret = RHN_ERROR_PARAM;
  }
  return ret;
}

jwks_t * r_jws_get_jwks_privkey(jws_t * jws) {
  if (jws != NULL) {
    return r_jwks_copy(jws->jwks_privkey);
  } else {
    return NULL;
  }
}

jwks_t * r_jws_get_jwks_pubkey(jws_t * jws) {
  if (jws != NULL) {
    return r_jwks_copy(jws->jwks_pubkey);
  } else {
    return NULL;
  }
}

int r_jws_parse(jws_t * jws, const char * jws_str, int x5u_flags) {
  return r_jws_parsen(jws, jws_str, o_strlen(jws_str), x5u_flags);
}

int r_jws_parsen(jws_t * jws, const char * jws_str, size_t jws_str_len, int x5u_flags) {
  return r_jws_advanced_parsen(jws, jws_str, jws_str_len, R_PARSE_HEADER_ALL, x5u_flags);
}

int r_jws_parse_unsecure(jws_t * jws, const char * jws_str, int x5u_flags) {
  return r_jws_parsen_unsecure(jws, jws_str, o_strlen(jws_str), x5u_flags);
}

int r_jws_parsen_unsecure(jws_t * jws, const char * jws_str, size_t jws_str_len, int x5u_flags) {
  return r_jws_advanced_parsen(jws, jws_str, jws_str_len, R_PARSE_ALL, x5u_flags);
}

int r_jws_advanced_parse(jws_t * jws, const char * jws_str, uint32_t parse_flags, int x5u_flags) {
  return r_jws_advanced_parsen(jws, jws_str, o_strlen(jws_str), parse_flags, x5u_flags);
}

int r_jws_advanced_parsen(jws_t * jws, const char * jws_str, size_t jws_str_len, uint32_t parse_flags, int x5u_flags) {
  int ret;
  char * str = (char *)jws_str;

  if (jws != NULL && str != NULL && jws_str_len) {
    while(isspace((unsigned char)*str) && jws_str_len) {
      str++;
      jws_str_len--;
    }

    if (0 == o_strncmp("ey", str, 2)) {
      ret = r_jws_advanced_compact_parsen(jws, jws_str, jws_str_len, parse_flags, x5u_flags);
    } else if (*str == '{') {
      ret = r_jws_advanced_parsen_json_str(jws, jws_str, jws_str_len, parse_flags, x5u_flags);
    } else {
      ret = RHN_ERROR_PARAM;
    }
  } else {
    ret = RHN_ERROR_PARAM;
  }
  return ret;
}

int r_jws_compact_parsen(jws_t * jws, const char * jws_str, size_t jws_str_len, int x5u_flags) {
  int ret = r_jws_compact_parsen_unsecure(jws, jws_str, jws_str_len, x5u_flags);
  if (ret == RHN_OK) {
    if (r_jws_get_alg(jws) != R_JWA_ALG_NONE) {
      return RHN_OK;
    } else {
      return RHN_ERROR_INVALID;
    }
  } else {
    return ret;
  }
}

int r_jws_compact_parse(jws_t * jws, const char * jws_str, int x5u_flags) {
  return r_jws_compact_parsen_unsecure(jws, jws_str, o_strlen(jws_str), x5u_flags);
}

int r_jws_compact_parsen_unsecure(jws_t * jws, const char * jws_str, size_t jws_str_len, int x5u_flags) {
  return r_jws_advanced_compact_parsen(jws, jws_str, jws_str_len, R_PARSE_ALL, x5u_flags);
}

int r_jws_compact_parse_unsecure(jws_t * jws, const char * jws_str, int x5u_flags) {
  return r_jws_compact_parsen_unsecure(jws, jws_str, o_strlen(jws_str), x5u_flags);
}

int r_jws_advanced_compact_parse(jws_t * jws, const char * jws_str, uint32_t parse_flags, int x5u_flags) {
  return r_jws_advanced_compact_parsen(jws, jws_str, o_strlen(jws_str), parse_flags, x5u_flags);
}

int r_jws_advanced_compact_parsen(jws_t * jws, const char * jws_str, size_t jws_str_len, uint32_t parse_flags, int x5u_flags) {
  int ret;
  char ** str_array = NULL;
  char * token = NULL;
  size_t split_size = 0, unzip_len = 0;
  json_t * j_header = NULL;
  struct _o_datum dat_header = {0, NULL}, dat_payload = {0, NULL};
  unsigned char * unzip = NULL;

  if (jws != NULL && jws_str != NULL && jws_str_len) {
    token = o_strndup(jws_str, jws_str_len);
    if ((split_size = split_string(token, ".", &str_array)) == 2 || split_size == 3) {
      // Check if all first 2 elements are base64url
      if (o_base64url_decode_alloc((unsigned char *)str_array[0], o_strlen(str_array[0]), &dat_header) &&
          o_base64url_decode_alloc((unsigned char *)str_array[1], o_strlen(str_array[1]), &dat_payload)) {
        ret = RHN_OK;
        do {
          // Decode header
          j_header = json_loadb((const char*)dat_header.data, dat_header.size, JSON_DECODE_ANY, NULL);
          if (r_jws_extract_header(jws, j_header, parse_flags, x5u_flags) != RHN_OK) {
            y_log_message(Y_LOG_LEVEL_ERROR, "r_jws_advanced_compact_parsen - error extracting header params");
            ret = RHN_ERROR_PARAM;
            break;
          }
          json_decref(jws->j_header);

          jws->j_header = json_incref(j_header);

          if (!(parse_flags&R_PARSE_UNSIGNED)) {
            if (r_jws_get_alg(jws) == R_JWA_ALG_NONE) {
              y_log_message(Y_LOG_LEVEL_DEBUG, "r_jws_advanced_compact_parsen - error unsigned jws");
              ret = RHN_ERROR_INVALID;
              break;
            }
          }

          // Decode payload
          if (0 == o_strcmp("DEF", r_jws_get_header_str_value(jws, "zip"))) {
            if (_r_inflate_payload(dat_payload.data, dat_payload.size, &unzip, &unzip_len) != RHN_OK) {
              y_log_message(Y_LOG_LEVEL_ERROR, "r_jws_advanced_compact_parsen - error _r_inflate_payload");
              ret = RHN_ERROR_PARAM;
              break;
            }
            if (r_jws_set_payload(jws, unzip, unzip_len) != RHN_OK) {
              y_log_message(Y_LOG_LEVEL_ERROR, "r_jws_advanced_compact_parsen - error r_jws_set_payload");
              ret = RHN_ERROR_PARAM;
              break;
            }
          } else {
            if (r_jws_set_payload(jws, dat_payload.data, dat_payload.size) != RHN_OK) {
              y_log_message(Y_LOG_LEVEL_ERROR, "r_jws_advanced_compact_parsen - Error r_jws_set_payload");
              ret = RHN_ERROR;
              break;
            }
          }

          o_free(jws->header_b64url);
          jws->header_b64url = (unsigned char *)o_strdup(str_array[0]);

          o_free(jws->signature_b64url);
          jws->signature_b64url = NULL;
          if (str_array[2] != NULL) {
            jws->signature_b64url = (unsigned char *)o_strdup(str_array[2]);
          }
          if (r_jws_get_alg(jws) != R_JWA_ALG_NONE && o_strnullempty(str_array[2])) {
            y_log_message(Y_LOG_LEVEL_ERROR, "r_jws_advanced_compact_parsen - error invalid signature length");
            ret = RHN_ERROR_PARAM;
            break;
          }
        } while (0);
        json_decref(j_header);
        o_free(unzip);
      } else {
        y_log_message(Y_LOG_LEVEL_ERROR, "r_jws_advanced_compact_parsen - error decoding jws from base64url format");
        ret = RHN_ERROR_PARAM;
      }
      o_free(dat_header.data);
      o_free(dat_payload.data);
    } else {
      y_log_message(Y_LOG_LEVEL_ERROR, "r_jws_advanced_compact_parsen - jws_str invalid format");
      ret = RHN_ERROR_PARAM;
    }
    free_string_array(str_array);
    o_free(token);
  } else {
    ret = RHN_ERROR_PARAM;
  }
  return ret;
}

int r_jws_parse_json_t(jws_t * jws, json_t * jws_json, int x5u_flags) {
  return r_jws_advanced_parse_json_t(jws, jws_json, R_PARSE_HEADER_ALL, x5u_flags);
}

int r_jws_parsen_json_str(jws_t * jws, const char * jws_json_str, size_t jws_str_len, int x5u_flags) {
  json_t * jws_json = NULL;
  int ret;

  jws_json = json_loadb(jws_json_str, jws_str_len, JSON_DECODE_ANY, NULL);
  ret = r_jws_parse_json_t(jws, jws_json, x5u_flags);
  json_decref(jws_json);

  return ret;
}

int r_jws_parse_json_str(jws_t * jws, const char * jws_json_str, int x5u_flags) {
  return r_jws_parsen_json_str(jws, jws_json_str, o_strlen(jws_json_str), x5u_flags);
}

int r_jws_advanced_parse_json_str(jws_t * jws, const char * jws_json_str, uint32_t parse_flags, int x5u_flags) {
  return r_jws_advanced_parsen_json_str(jws, jws_json_str, o_strlen(jws_json_str), parse_flags, x5u_flags);
}

int r_jws_advanced_parsen_json_str(jws_t * jws, const char * jws_json_str, size_t jws_json_str_len, uint32_t parse_flags, int x5u_flags) {
  json_t * jws_json = NULL;
  int ret;

  jws_json = json_loadb(jws_json_str, jws_json_str_len, JSON_DECODE_ANY, NULL);
  ret = r_jws_advanced_parse_json_t(jws, jws_json, parse_flags, x5u_flags);
  json_decref(jws_json);

  return ret;
}

int r_jws_advanced_parse_json_t(jws_t * jws, json_t * jws_json, uint32_t parse_flags, int x5u_flags) {
  int ret;
  size_t index = 0, signature_len = 0, header_len = 0;
  char * str_header = NULL;
  json_t * j_header = NULL, * j_element = NULL;
  struct _o_datum dat_header = {0, NULL}, dat_payload = {0, NULL};

  if (jws != NULL && json_is_object(jws_json)) {
    if (json_string_length(json_object_get(jws_json, "payload"))) {
      if (json_string_length(json_object_get(jws_json, "protected"))) {
        // Mode flattened - 1 signature maximum
        ret = RHN_OK;
        jws->token_mode = R_JSON_MODE_FLATTENED;

        do {
          json_decref(jws->j_json_serialization);
          if ((jws->j_json_serialization = json_deep_copy(jws_json)) == NULL) {
            y_log_message(Y_LOG_LEVEL_ERROR, "r_jws_parse_json_t - Error setting j_json_serialization");
            ret = RHN_ERROR;
            break;
          }

          o_free(jws->header_b64url);
          if ((jws->header_b64url = (unsigned char *)o_strdup(json_string_value(json_object_get(jws_json, "protected")))) == NULL) {
            y_log_message(Y_LOG_LEVEL_ERROR, "r_jws_parse_json_t - Error setting header_b64url");
            ret = RHN_ERROR;
            break;
          }

          o_free(jws->payload_b64url);
          if ((jws->payload_b64url = (unsigned char *)o_strdup(json_string_value(json_object_get(jws_json, "payload")))) == NULL) {
            y_log_message(Y_LOG_LEVEL_ERROR, "r_jws_parse_json_t - Error setting payload_b64url");
            ret = RHN_ERROR;
            break;
          }

          o_free(jws->signature_b64url);
          if (json_string_length(json_object_get(jws_json, "signature"))) {
            if ((jws->signature_b64url = (unsigned char *)o_strdup(json_string_value(json_object_get(jws_json, "signature")))) == NULL) {
              y_log_message(Y_LOG_LEVEL_ERROR, "r_jws_parse_json_t - Error setting signature_b64url");
              ret = RHN_ERROR;
              break;
            }
            if (!o_base64url_decode((unsigned char *)jws->signature_b64url, o_strlen((const char *)jws->signature_b64url), NULL, &signature_len)) {
              y_log_message(Y_LOG_LEVEL_ERROR, "r_jws_parse_json_t - Invalid JWS, signature not valid base64url format");
              ret = RHN_ERROR_PARAM;
              break;
            }
          } else {
            jws->signature_b64url = NULL;
          }

          // Decode header
          if (!o_base64url_decode_alloc((unsigned char *)jws->header_b64url, o_strlen((const char *)jws->header_b64url), &dat_header)) {
            y_log_message(Y_LOG_LEVEL_ERROR, "r_jws_parse_json_t - Error decoding str_header");
            ret = RHN_ERROR_PARAM;
            break;
          }

          j_header = json_loadb((const char*)dat_header.data, dat_header.size, JSON_DECODE_ANY, NULL);
          if (r_jws_extract_header(jws, j_header, parse_flags, x5u_flags) != RHN_OK) {
            y_log_message(Y_LOG_LEVEL_ERROR, "r_jws_parse_json_t - Error extracting header params");
            ret = RHN_ERROR_PARAM;
            break;
          }
          json_decref(jws->j_header);

          jws->j_header = json_incref(j_header);

          // Decode payload
          if (!o_base64url_decode_alloc((unsigned char *)jws->payload_b64url, o_strlen((const char *)jws->payload_b64url), &dat_payload)) {
            y_log_message(Y_LOG_LEVEL_ERROR, "r_jws_parse_json_t - Error decoding payload");
            ret = RHN_ERROR_PARAM;
            break;
          }

          if (r_jws_set_payload(jws, dat_payload.data, dat_payload.size) != RHN_OK) {
            y_log_message(Y_LOG_LEVEL_ERROR, "r_jws_parse_json_t - Error r_jws_set_payload");
            ret = RHN_ERROR;
            break;
          }

          if (r_jws_extract_header(jws, json_object_get(jws_json, "header"), parse_flags, x5u_flags) != RHN_OK) {
            y_log_message(Y_LOG_LEVEL_ERROR, "r_jws_parse_json_t - Error extracting header params");
            ret = RHN_ERROR_PARAM;
            break;
          }
        } while (0);
        json_decref(j_header);
        o_free(dat_header.data);
        o_free(dat_payload.data);

      } else {
        ret = RHN_OK;
        if (json_array_size(json_object_get(jws_json, "signatures"))) {
          json_array_foreach(json_object_get(jws_json, "signatures"), index, j_element) {
            if (!json_string_length(json_object_get(j_element, "protected")) || !json_string_length(json_object_get(j_element, "signature"))) {
              y_log_message(Y_LOG_LEVEL_ERROR, "r_jws_parse_json_t - Error invalid format, a signature object must contain string elements 'protected' and 'signature'");
              ret = RHN_ERROR_PARAM;
              break;
            }

            if (json_object_get(j_element, "header") && !json_is_object(json_object_get(j_element, "header"))) {
              y_log_message(Y_LOG_LEVEL_ERROR, "r_jws_parse_json_t - Error invalid format, the 'header property in a signature object must be a JSON object");
              ret = RHN_ERROR_PARAM;
              break;
            }

            if (!o_base64url_decode((const unsigned char *)json_string_value(json_object_get(j_element, "protected")), json_string_length(json_object_get(j_element, "protected")), NULL, &header_len)) {
              y_log_message(Y_LOG_LEVEL_ERROR, "r_jws_parse_json_t - Error header base64url format");
              ret = RHN_ERROR_PARAM;
              break;
            }

            if (!o_base64url_decode((const unsigned char *)json_string_value(json_object_get(j_element, "signature")), json_string_length(json_object_get(j_element, "signature")), NULL, &signature_len)) {
              y_log_message(Y_LOG_LEVEL_ERROR, "r_jws_parse_json_t - Error signature base64url format");
              ret = RHN_ERROR_PARAM;
              break;
            }

          }
        } else {
          y_log_message(Y_LOG_LEVEL_ERROR, "r_jws_parse_json_t - Error invalid format, signatures must be a JSON array");
          ret = RHN_ERROR_PARAM;
        }

        if (ret == RHN_OK) {
          // Mode general - multiple signatures allowed
          jws->token_mode = R_JSON_MODE_GENERAL;

          do {
            o_free(jws->header_b64url);
            jws->header_b64url = NULL;
            o_free(jws->signature_b64url);
            jws->signature_b64url = NULL;

            json_decref(jws->j_json_serialization);
            if ((jws->j_json_serialization = json_deep_copy(jws_json)) == NULL) {
              y_log_message(Y_LOG_LEVEL_ERROR, "r_jws_parse_json_t - Error setting j_json_serialization");
              ret = RHN_ERROR;
              break;
            }

            o_free(jws->payload_b64url);
            if ((jws->payload_b64url = (unsigned char *)o_strdup(json_string_value(json_object_get(jws_json, "payload")))) == NULL) {
              y_log_message(Y_LOG_LEVEL_ERROR, "r_jws_parse_json_t - Error setting payload_b64url");
              ret = RHN_ERROR;
              break;
            }

            // Decode payload
            if (!o_base64url_decode_alloc((unsigned char *)jws->payload_b64url, o_strlen((const char *)jws->payload_b64url), &dat_payload)) {
              y_log_message(Y_LOG_LEVEL_ERROR, "r_jws_parse_json_t - error decoding jws->payload");
              ret = RHN_ERROR_PARAM;
              break;
            }

            if (r_jws_set_payload(jws, dat_payload.data, dat_payload.size) != RHN_OK) {
              y_log_message(Y_LOG_LEVEL_ERROR, "r_jws_parse_json_t - Error r_jws_set_payload");
              ret = RHN_ERROR;
              break;
            }
          } while (0);
          json_decref(j_header);
          o_free(str_header);
          o_free(dat_payload.data);
        }

      }
    } else {
      y_log_message(Y_LOG_LEVEL_ERROR, "r_jws_parse_json_t - Error payload missing");
      ret = RHN_ERROR_PARAM;
    }
  } else {
    y_log_message(Y_LOG_LEVEL_ERROR, "r_jws_parse_json_t - Error input parameters");
    ret = RHN_ERROR_PARAM;
  }
  return ret;
}

jws_t * r_jws_quick_parse(const char * jws_str, uint32_t parse_flags, int x5u_flags) {
  return r_jws_quick_parsen(jws_str, o_strlen(jws_str), parse_flags, x5u_flags);
}

jws_t * r_jws_quick_parsen(const char * jws_str, size_t jws_str_len, uint32_t parse_flags, int x5u_flags) {
  jws_t * jws = NULL;
  int ret;

  if (r_jws_init(&jws) == RHN_OK) {
    ret = r_jws_advanced_parsen(jws, jws_str, jws_str_len, parse_flags, x5u_flags);
    if (ret != RHN_OK) {
      r_jws_free(jws);
      jws = NULL;
    }
  } else {
    r_jws_free(jws);
    jws = NULL;
  }
  return jws;
}

int r_jws_verify_signature(jws_t * jws, jwk_t * jwk_pubkey, int x5u_flags) {
  int ret, res;
  jwk_t * jwk = NULL, * cur_jwk;
  const char * kid;
  json_t * j_signature = NULL, * j_header;
  size_t index = 0, i;

  if (jws != NULL) {
    if (jwk_pubkey != NULL) {
      jwk = r_jwk_copy(jwk_pubkey);
    } else {
      if ((kid = r_jws_get_header_str_value(jws, "kid")) != NULL || (jws->token_mode == R_JSON_MODE_FLATTENED && (kid = json_string_value(json_object_get(json_object_get(jws->j_json_serialization, "header"), "kid"))) != NULL)) {
        jwk = r_jwks_get_by_kid(jws->jwks_pubkey, kid);
      } else if (r_jwks_size(jws->jwks_pubkey) == 1) {
        jwk = r_jwks_get_at(jws->jwks_pubkey, 0);
      }
    }
  }

  if (jws != NULL) {
    if (jws->token_mode == R_JSON_MODE_GENERAL) {
      ret = RHN_ERROR_INVALID;
      o_free(jws->header_b64url);
      o_free(jws->signature_b64url);
      json_array_foreach(json_object_get(jws->j_json_serialization, "signatures"), index, j_signature) {
        jws->header_b64url = (unsigned char *)json_string_value(json_object_get(j_signature, "protected"));
        jws->signature_b64url = (unsigned char *)json_string_value(json_object_get(j_signature, "signature"));
        kid = json_string_value(json_object_get(json_object_get(j_signature, "header"), "kid"));
        if ((j_header = r_jws_parse_protected((const unsigned char *)json_string_value(json_object_get(j_signature, "protected")))) != NULL) {
          res = r_jws_extract_header(jws, j_header, R_PARSE_NONE, x5u_flags);
          json_decref(j_header);
          if (res == RHN_OK) {
            if (!o_strnullempty(kid)) {
              if (jwk_pubkey != NULL) {
                ret = _r_verify_signature(jws, jwk, jws->alg, x5u_flags);
              } else {
                if ((cur_jwk = r_jwks_get_by_kid(jws->jwks_pubkey, kid)) != NULL) {
                  ret = _r_verify_signature(jws, cur_jwk, jws->alg, x5u_flags);
                  r_jwk_free(cur_jwk);
                }
              }
              if (ret != RHN_ERROR_INVALID) {
                break;
              }
            } else {
              if (jwk_pubkey != NULL) {
                if ((ret = _r_verify_signature(jws, jwk_pubkey, jws->alg, x5u_flags)) != RHN_ERROR_INVALID) {
                  break;
                }
              } else if (r_jwks_size(jws->jwks_pubkey)) {
                for (i=0; i<r_jwks_size(jws->jwks_pubkey); i++) {
                  cur_jwk = r_jwks_get_at(jws->jwks_pubkey, i);
                  ret = _r_verify_signature(jws, cur_jwk, jws->alg, x5u_flags);
                  r_jwk_free(cur_jwk);
                  if (ret != RHN_ERROR_INVALID) {
                    break;
                  }
                }
                if (ret != RHN_ERROR_INVALID) {
                  break;
                }
              }
            }
          } else {
            ret = RHN_ERROR;
            break;
          }
        } else {
          ret = RHN_ERROR;
          break;
        }
      }
      jws->header_b64url = NULL;
      jws->signature_b64url = NULL;
    } else {
      if (r_jws_set_token_values(jws, 0) == RHN_OK && jws->signature_b64url != NULL) {
        if (jwk != NULL) {
          ret = _r_verify_signature(jws, jwk, jws->alg, x5u_flags);
        } else {
          ret = RHN_ERROR_INVALID;
        }
      } else {
        ret = RHN_ERROR_PARAM;
      }
    }
  } else {
    ret = RHN_ERROR_PARAM;
  }
  r_jwk_free(jwk);
  return ret;
}

char * r_jws_serialize(jws_t * jws, jwk_t * jwk_privkey, int x5u_flags) {
  if (r_jws_get_alg(jws) != R_JWA_ALG_NONE) {
    return r_jws_serialize_unsecure(jws, jwk_privkey, x5u_flags);
  } else {
    return NULL;
  }
}

char * r_jws_serialize_unsecure(jws_t * jws, jwk_t * jwk_privkey, int x5u_flags) {
  jwk_t * jwk = NULL;
  char * jws_str = NULL;
  jwa_alg alg;

  if (jws != NULL) {
    if (jwk_privkey != NULL) {
      jwk = r_jwk_copy(jwk_privkey);
      if (jws->alg == R_JWA_ALG_UNKNOWN && (alg = r_str_to_jwa_alg(r_jwk_get_property_str(jwk, "alg"))) != R_JWA_ALG_NONE && alg != R_JWA_ALG_UNKNOWN) {
        r_jws_set_alg(jws, alg);
      }
    } else {
      if (r_jws_get_header_str_value(jws, "kid") != NULL) {
        jwk = r_jwks_get_by_kid(jws->jwks_privkey, r_jws_get_header_str_value(jws, "kid"));
      } else if (jws != NULL && r_jwks_size(jws->jwks_privkey) == 1) {
        jwk = r_jwks_get_at(jws->jwks_privkey, 0);
      }
      if (jws->alg == R_JWA_ALG_UNKNOWN && (alg = r_str_to_jwa_alg(r_jwk_get_property_str(jwk, "alg"))) != R_JWA_ALG_NONE && alg != R_JWA_ALG_UNKNOWN) {
        r_jws_set_alg(jws, alg);
      }
    }

    if (r_jwk_get_property_str(jwk, "kid") != NULL && r_jws_get_header_str_value(jws, "kid") == NULL) {
      r_jws_set_header_str_value(jws, "kid", r_jwk_get_property_str(jwk, "kid"));
    }

    o_free(jws->signature_b64url);
    if (r_jws_set_token_values(jws, 1) == RHN_OK) {
      jws->signature_b64url = _r_generate_signature(jws, jwk, jws->alg, x5u_flags);
      if (jws->signature_b64url != NULL) {
        jws_str = msprintf("%s.%s.%s", jws->header_b64url, jws->payload_b64url, jws->signature_b64url);
      } else {
        y_log_message(Y_LOG_LEVEL_ERROR, "r_jws_serialize - No signature");
      }
    } else {
      y_log_message(Y_LOG_LEVEL_ERROR, "r_jws_serialize - Error r_jws_set_token_values");
    }
  } else {
    y_log_message(Y_LOG_LEVEL_ERROR, "r_jws_serialize - Error input parameters");
  }

  r_jwk_free(jwk);
  return jws_str;
}

char * r_jws_serialize_json_str(jws_t * jws, jwks_t * jwks_privkey, int x5u_flags, int mode) {
  json_t * j_result = r_jws_serialize_json_t(jws, jwks_privkey, x5u_flags, mode);
  char * str_result = json_dumps(j_result, JSON_COMPACT);
  json_decref(j_result);
  return str_result;
}

json_t * r_jws_serialize_json_t(jws_t * jws, jwks_t * jwks_privkey, int x5u_flags, int mode) {
  json_t * j_return = NULL, * j_signature;
  jwk_t * jwk = NULL;
  jwa_alg alg;
  const char * kid;
  unsigned char * signature = NULL;
  size_t i = 0;

  if (jwks_privkey == NULL) {
    jwks_privkey = jws->jwks_privkey;
  }
  if (jws != NULL && r_jwks_size(jwks_privkey)) {
    jws->token_mode = mode;
    if (mode == R_JSON_MODE_FLATTENED) {
      if ((kid = r_jws_get_header_str_value(jws, "kid")) != NULL) {
        jwk = r_jwks_get_by_kid(jwks_privkey, r_jws_get_header_str_value(jws, "kid"));
      } else {
        jwk = r_jwks_get_at(jwks_privkey, 0);
        kid = r_jwk_get_property_str(jwk, "kid");
      }
      if ((alg = jws->alg) == R_JWA_ALG_UNKNOWN && (alg = r_str_to_jwa_alg(r_jwk_get_property_str(jwk, "alg"))) != R_JWA_ALG_NONE && alg != R_JWA_ALG_UNKNOWN) {
        r_jws_set_alg(jws, alg);
      }
      if (r_jws_set_token_values(jws, 1) == RHN_OK) {
        if ((signature = _r_generate_signature(jws, jwk, alg, x5u_flags)) != NULL) {
          j_return = json_pack("{ssssss}", "payload", jws->payload_b64url, "protected", jws->header_b64url, "signature", signature);
          if (kid != NULL) {
            json_object_set_new(j_return, "header", json_pack("{ss}", "kid", kid));
          }
        } else {
          y_log_message(Y_LOG_LEVEL_ERROR, "r_jws_serialize_json_t - Error _r_generate_signature");
        }
        o_free(signature);
      } else {
        y_log_message(Y_LOG_LEVEL_ERROR, "r_jws_serialize_json_t - Error r_jws_set_header_value");
      }
      r_jwk_free(jwk);
    } else {
      if (r_jws_set_payload_value(jws, 1) == RHN_OK) {
        j_return = json_pack("{sss[]}", "payload", jws->payload_b64url, "signatures");
        for (i=0; i<r_jwks_size(jwks_privkey); i++) {
          jwk = r_jwks_get_at(jwks_privkey, i);
          if ((alg = r_str_to_jwa_alg(r_jwk_get_property_str(jwk, "alg"))) != R_JWA_ALG_NONE && alg != R_JWA_ALG_UNKNOWN) {
            kid = r_jwk_get_property_str(jwk, "kid");
            r_jws_set_alg(jws, alg);
            if (r_jws_set_header_value(jws, 1) == RHN_OK) {
              if ((signature = _r_generate_signature(jws, jwk, alg, x5u_flags)) != NULL) {
                j_signature = json_pack("{ssss}", "protected", jws->header_b64url, "signature", signature);
                if (kid != NULL) {
                  json_object_set_new(j_signature, "header", json_pack("{ss}", "kid", kid));
                }
                json_array_append_new(json_object_get(j_return, "signatures"), j_signature);
              } else {
                y_log_message(Y_LOG_LEVEL_ERROR, "r_jws_serialize_json_t - Error _r_generate_signature");
              }
              o_free(signature);
            } else {
              y_log_message(Y_LOG_LEVEL_ERROR, "r_jws_serialize_json_t - Error generating protected header at index %zu", i);
            }
          } else {
            y_log_message(Y_LOG_LEVEL_ERROR, "r_jws_serialize_json_t - Invalid jwk at index %zu, no alg specified", i);
          }
          r_jwk_free(jwk);
        }
      } else {
        y_log_message(Y_LOG_LEVEL_ERROR, "r_jws_serialize_json_t - Error r_jws_set_header_value");
      }
    }
    json_decref(jws->j_json_serialization);
    jws->j_json_serialization = json_deep_copy(j_return);
  } else {
    y_log_message(Y_LOG_LEVEL_ERROR, "r_jws_serialize_json_t - Error input parameters");
  }
  return j_return;
}

int r_jws_set_full_header_json_t(jws_t * jws, json_t * j_header) {
  int ret = RHN_OK;
  jwa_alg alg;

  if (jws != NULL && json_is_object(j_header)) {
    if (json_object_get(j_header, "alg") != NULL) {
      if ((alg = r_str_to_jwa_alg(json_string_value(json_object_get(j_header, "alg")))) != R_JWA_ALG_UNKNOWN) {
        jws->alg = alg;
      } else {
        y_log_message(Y_LOG_LEVEL_ERROR, "r_jws_set_full_header_json_t - Error invalid alg parameter");
        ret = RHN_ERROR_PARAM;
      }
    }
    if (ret == RHN_OK) {
      json_decref(jws->j_header);
      if ((jws->j_header = json_deep_copy(j_header)) == NULL) {
        y_log_message(Y_LOG_LEVEL_ERROR, "r_jws_set_full_header_json_t - Error setting header");
        ret = RHN_ERROR_MEMORY;
      }
    }
  } else {
    y_log_message(Y_LOG_LEVEL_ERROR, "r_jws_set_full_header_json_t - Error input parameters");
    ret = RHN_ERROR_PARAM;
  }
  return ret;
}

int r_jws_set_full_header_json_str(jws_t * jws, const char * str_header) {
  int ret;
  json_t * j_header = json_loads(str_header, JSON_DECODE_ANY, NULL);

  ret = r_jws_set_full_header_json_t(jws, j_header);
  json_decref(j_header);

  return ret;
}

int r_jws_set_properties(jws_t * jws, ...) {
  rhn_opt option;
  unsigned int ui_value;
  int ret = RHN_OK;
  int i_value;
  rhn_int_t r_value;
  const char * str_key, * str_value;
  json_t * j_value;
  const unsigned char * ustr_value;
  size_t size_value;
  jwk_t * jwk;
  jwks_t * jwks;
  gnutls_privkey_t privkey;
  gnutls_pubkey_t pubkey;
  va_list vl;

  if (jws != NULL) {
    va_start(vl, jws);
    for (option = va_arg(vl, rhn_opt); option != RHN_OPT_NONE && ret == RHN_OK; option = va_arg(vl, rhn_opt)) {
      switch (option) {
        case RHN_OPT_HEADER_INT_VALUE:
          str_key = va_arg(vl, const char *);
          i_value = va_arg(vl, int);
          ret = r_jws_set_header_int_value(jws, str_key, (rhn_int_t)i_value);
          break;
        case RHN_OPT_HEADER_RHN_INT_VALUE:
          str_key = va_arg(vl, const char *);
          r_value = va_arg(vl, rhn_int_t);
          ret = r_jws_set_header_int_value(jws, str_key, r_value);
          break;
        case RHN_OPT_HEADER_STR_VALUE:
          str_key = va_arg(vl, const char *);
          str_value = va_arg(vl, const char *);
          ret = r_jws_set_header_str_value(jws, str_key, str_value);
          break;
        case RHN_OPT_HEADER_JSON_T_VALUE:
          str_key = va_arg(vl, const char *);
          j_value = va_arg(vl, json_t *);
          ret = r_jws_set_header_json_t_value(jws, str_key, j_value);
          break;
        case RHN_OPT_HEADER_FULL_JSON_T:
          j_value = va_arg(vl, json_t *);
          ret = r_jws_set_full_header_json_t(jws, j_value);
          break;
        case RHN_OPT_HEADER_FULL_JSON_STR:
          str_value = va_arg(vl, const char *);
          ret = r_jws_set_full_header_json_str(jws, str_value);
          break;
        case RHN_OPT_PAYLOAD:
          ustr_value = va_arg(vl, const unsigned char *);
          size_value = va_arg(vl, size_t);
          ret = r_jws_set_payload(jws, ustr_value, size_value);
          break;
        case RHN_OPT_SIG_ALG:
          ui_value = va_arg(vl, unsigned int);
          ret = r_jws_set_alg(jws, (jwa_alg)ui_value);
          break;
        case RHN_OPT_VERIFY_KEY_JWK:
          jwk = va_arg(vl, jwk_t *);
          ret = r_jws_add_keys(jws, NULL, jwk);
          break;
        case RHN_OPT_VERIFY_KEY_JWKS:
          jwks = va_arg(vl, jwks_t *);
          ret = r_jws_add_jwks(jws, NULL, jwks);
          break;
        case RHN_OPT_VERIFY_KEY_GNUTLS:
          pubkey = va_arg(vl, gnutls_pubkey_t);
          ret = r_jws_add_keys_gnutls(jws, NULL, pubkey);
          break;
        case RHN_OPT_VERIFY_KEY_JSON_T:
          j_value = va_arg(vl, json_t *);
          ret = r_jws_add_keys_json_t(jws, NULL, j_value);
          break;
        case RHN_OPT_VERIFY_KEY_JSON_STR:
          str_value = va_arg(vl, const char *);
          ret = r_jws_add_keys_json_str(jws, NULL, str_value);
          break;
        case RHN_OPT_VERIFY_KEY_PEM_DER:
          ui_value = va_arg(vl, unsigned int);
          ustr_value = va_arg(vl, const unsigned char *);
          size_value = va_arg(vl, size_t);
          ret = r_jws_add_keys_pem_der(jws, (int)ui_value, NULL, 0, ustr_value, size_value);
          break;
        case RHN_OPT_SIGN_KEY_JWK:
          jwk = va_arg(vl, jwk_t *);
          ret = r_jws_add_keys(jws, jwk, NULL);
          break;
        case RHN_OPT_SIGN_KEY_JWKS:
          jwks = va_arg(vl, jwks_t *);
          ret = r_jws_add_jwks(jws, jwks, NULL);
          break;
        case RHN_OPT_SIGN_KEY_GNUTLS:
          privkey = va_arg(vl, gnutls_privkey_t);
          ret = r_jws_add_keys_gnutls(jws, privkey, NULL);
          break;
        case RHN_OPT_SIGN_KEY_JSON_T:
          j_value = va_arg(vl, json_t *);
          ret = r_jws_add_keys_json_t(jws, j_value, NULL);
          break;
        case RHN_OPT_SIGN_KEY_JSON_STR:
          str_value = va_arg(vl, const char *);
          ret = r_jws_add_keys_json_str(jws, str_value, NULL);
          break;
        case RHN_OPT_SIGN_KEY_PEM_DER:
          ui_value = va_arg(vl, unsigned int);
          ustr_value = va_arg(vl, const unsigned char *);
          size_value = va_arg(vl, size_t);
          ret = r_jws_add_keys_pem_der(jws, (int)ui_value, ustr_value, size_value, NULL, 0);
          break;
        default:
          ret = RHN_ERROR_PARAM;
          break;
      }
    }
    va_end(vl);
  } else {
    y_log_message(Y_LOG_LEVEL_ERROR, "r_jws_set_properties - Error input parameter");
    ret = RHN_ERROR_PARAM;
  }
  return ret;
}
