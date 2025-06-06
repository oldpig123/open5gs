/*
 * Copyright (C) 2019,2020 by Sukchan Lee <acetcom@gmail.com>
 *
 * This file is part of Open5GS.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

 #include "test-common.h"
 #include <time.h>
 
 static void test1_func(abts_case *tc, void *data)
 {
     int rv;
     ogs_socknode_t *ngap;
     ogs_socknode_t *gtpu;
     ogs_pkbuf_t *gmmbuf;
     ogs_pkbuf_t *gsmbuf;
     ogs_pkbuf_t *nasbuf;
     ogs_pkbuf_t *sendbuf;
     ogs_pkbuf_t *recvbuf;
     ogs_ngap_message_t message;
     int i;
 
     ogs_nas_5gs_mobile_identity_suci_t mobile_identity_suci;
     test_ue_t *test_ue = NULL;
     test_sess_t *sess = NULL;
     test_bearer_t *qos_flow = NULL;
 
     bson_t *doc = NULL;
 
     /* Setup Test UE & Session Context */
     memset(&mobile_identity_suci, 0, sizeof(mobile_identity_suci));
 
     mobile_identity_suci.h.supi_format = OGS_NAS_5GS_SUPI_FORMAT_IMSI;
     mobile_identity_suci.h.type = OGS_NAS_5GS_MOBILE_IDENTITY_SUCI;
     mobile_identity_suci.routing_indicator1 = 0;
     mobile_identity_suci.routing_indicator2 = 0xf;
     mobile_identity_suci.routing_indicator3 = 0xf;
     mobile_identity_suci.routing_indicator4 = 0xf;
     mobile_identity_suci.protection_scheme_id = OGS_PROTECTION_SCHEME_NULL;
     mobile_identity_suci.home_network_pki_value = 0;
 
     test_ue = test_ue_add_by_suci(&mobile_identity_suci, "0000203190");
     ogs_assert(test_ue);
 
     test_ue->nr_cgi.cell_id = 0x40001;
 
     test_ue->nas.registration.tsc = 0;
     test_ue->nas.registration.ksi = OGS_NAS_KSI_NO_KEY_IS_AVAILABLE;
     test_ue->nas.registration.follow_on_request = 1;
     test_ue->nas.registration.value = OGS_NAS_5GS_REGISTRATION_TYPE_INITIAL;
 
     test_ue->k_string = "465b5ce8b199b49faa5f0a2ee238a6bc";
     test_ue->opc_string = "e8ed289deba952e4283b54e88e6183ca";
     
 
  /* gNB connects to AMF */
  ngap = testngap_client(AF_INET);
  ABTS_PTR_NOTNULL(tc, ngap);
 
  /* gNB connects to UPF */
  gtpu = test_gtpu_server(1, AF_INET);
  ABTS_PTR_NOTNULL(tc, gtpu);
 
  /* Send NG-Setup Reqeust */
  sendbuf = testngap_build_ng_setup_request(0x4000, 22);
  ABTS_PTR_NOTNULL(tc, sendbuf);
  rv = testgnb_ngap_send(ngap, sendbuf);
  ABTS_INT_EQUAL(tc, OGS_OK, rv);
  
  /* Receive NG-Setup Response */
  recvbuf = testgnb_ngap_read(ngap);
  ABTS_PTR_NOTNULL(tc, recvbuf);
  testngap_recv(test_ue, recvbuf);
  
  /********** Insert Subscriber in Database */
  doc = test_db_new_simple(test_ue);
  ABTS_PTR_NOTNULL(tc, doc);
  ABTS_INT_EQUAL(tc, OGS_OK, test_db_insert_ue(test_ue, doc));
 
 
  while(1)
  {
      
      srand(time(NULL));
      // 生成 10 個隨機數
      int cnt = 0;
      char str[11];
      while (cnt < 10) 
      {
          cnt++;
          // 生成隨機的十位數
          int random_number = rand() % 10000000000;
          // 使用 %010d 格式化輸出到字符串，確保有 10 位數，不足的前面補零
          snprintf(str, sizeof(str), "%010d", random_number);
      }
      
      //////////////////////////
      ////////bad idea//////////
      
      test_ue = test_ue_add_by_suci(&mobile_identity_suci, str);
      //printf("imsi-99970-%s is registing.\n",str);
      
      //////////////////////////
      //////////////////////////
     
      /* Send Registration request */
      test_ue->registration_request_param.guti = 1;
      gmmbuf = testgmm_build_registration_request(test_ue, NULL, false, false);
      ABTS_PTR_NOTNULL(tc, gmmbuf);
 
      test_ue->registration_request_param.gmm_capability = 1;
      test_ue->registration_request_param.s1_ue_network_capability = 1;
      test_ue->registration_request_param.requested_nssai = 1;
      test_ue->registration_request_param.last_visited_registered_tai = 1;
      test_ue->registration_request_param.ue_usage_setting = 1;
      nasbuf = testgmm_build_registration_request(test_ue, NULL, false, false);
      ABTS_PTR_NOTNULL(tc, nasbuf);
 
      sendbuf = testngap_build_initial_ue_message(test_ue, gmmbuf,
      NGAP_RRCEstablishmentCause_mo_Signalling, false, true);
      ABTS_PTR_NOTNULL(tc, sendbuf);
      rv = testgnb_ngap_send(ngap, sendbuf);
      ABTS_INT_EQUAL(tc, OGS_OK, rv);
 
      /* Receive Identity request */
      recvbuf = testgnb_ngap_read(ngap);
      ABTS_PTR_NOTNULL(tc, recvbuf);
      testngap_recv(test_ue, recvbuf);
 
      /* Send Identity response */
      gmmbuf = testgmm_build_identity_response(test_ue);
      ABTS_PTR_NOTNULL(tc, gmmbuf);
      sendbuf = testngap_build_uplink_nas_transport(test_ue, gmmbuf);
      ABTS_PTR_NOTNULL(tc, sendbuf);
      rv = testgnb_ngap_send(ngap, sendbuf);
      ABTS_INT_EQUAL(tc, OGS_OK, rv);
  }
 
  /********** Remove Subscriber in Database */
  ABTS_INT_EQUAL(tc, OGS_OK, test_db_remove_ue(test_ue));
 
  /* gNB disonncect from UPF */
  testgnb_gtpu_close(gtpu);
 
  /* gNB disonncect from AMF */
  testgnb_ngap_close(ngap);
 
  /* Clear Test UE Context */
  test_ue_remove(test_ue);
 }
 
 abts_suite *test_simple(abts_suite *suite)
 {
     suite = ADD_SUITE(suite)
 
     abts_run_test(suite, test1_func, NULL);
 
     return suite;
 }
 