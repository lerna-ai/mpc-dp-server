/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   main.cpp
 * Author: gkellaris
 *
 * Created on August 24, 2020, 2:01 PM
 */

//SSL-Server.c

#include <errno.h>
#include <unistd.h>
#include <malloc.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <resolv.h>
#include "openssl/ssl.h"
#include "openssl/err.h"
#include <pthread.h>
#include <unordered_map>
#include <libxml/parser.h>
#include <libxml/tree.h>
#include <libxml/xmlstring.h>
#include <openssl/rand.h>
#include <mutex>
#include <list>
#include <math.h>


#define FAIL    -1
pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;
std::unordered_map<int, std::unordered_map<int, std::pair<double, int>>> shares;
std::mutex shares_mutex;
std::unordered_map<unsigned int, std::unordered_map<std::string, std::string>> jobs;
std::mutex jobs_mutex;

int OpenListener(int port) {
    int sd;
    struct sockaddr_in addr;

    sd = socket(PF_INET, SOCK_STREAM, 0);
    bzero(&addr, sizeof (addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = INADDR_ANY;
    if (bind(sd, (struct sockaddr*) &addr, sizeof (addr)) != 0) {
        perror("can't bind port");
        abort();
    }
    if (listen(sd, 50) != 0) {
        perror("Can't configure listening port");
        abort();
    }
    return sd;
}

SSL_CTX* InitServerCTX(void) {
    const SSL_METHOD *method;
    SSL_CTX *ctx;

    OpenSSL_add_all_algorithms(); /* load & register all cryptos, etc. */
    SSL_load_error_strings(); /* load all error messages */
    method = TLS_server_method(); /* create new server-method instance */
    ctx = SSL_CTX_new(method); /* create new context from method */
    if (ctx == NULL) {
        ERR_print_errors_fp(stderr);
        abort();
    }
    return ctx;
}

void LoadCertificates(SSL_CTX* ctx, char* CertFile, char* KeyFile) {
    //New lines 
    if (SSL_CTX_load_verify_locations(ctx, CertFile, KeyFile) != 1)
        ERR_print_errors_fp(stderr);

    if (SSL_CTX_set_default_verify_paths(ctx) != 1)
        ERR_print_errors_fp(stderr);
    //End new lines

    /* set the local certificate from CertFile */
    if (SSL_CTX_use_certificate_file(ctx, CertFile, SSL_FILETYPE_PEM) <= 0) {
        ERR_print_errors_fp(stderr);
        abort();
    }
    /* set the private key from KeyFile (may be the same as CertFile) */
    if (SSL_CTX_use_PrivateKey_file(ctx, KeyFile, SSL_FILETYPE_PEM) <= 0) {
        ERR_print_errors_fp(stderr);
        abort();
    }
    /* verify private key */
    if (!SSL_CTX_check_private_key(ctx)) {
        fprintf(stderr, "Private key does not match the public certificate\n");
        abort();
    }

    //New lines - Force the client-side have a certificate
    SSL_CTX_set_verify(ctx, SSL_VERIFY_PEER, NULL);
    SSL_CTX_set_verify_depth(ctx, 4);
    //End new lines

    // not issue stateless session tickets
    SSL_CTX_set_options(ctx, SSL_OP_NO_TICKET);
}

bool ShowCerts(SSL* ssl) {
    X509 *cert;
    char *line;

    cert = SSL_get_peer_certificate(ssl); /* Get certificates (if available) */
    if (cert != NULL) {
        //printf("Client certificates:\n");
        line = X509_NAME_oneline(X509_get_subject_name(cert), 0, 0);
        //printf("Subject: %s\n", line);
        free(line);
        line = X509_NAME_oneline(X509_get_issuer_name(cert), 0, 0);
        printf("Issuer: %s\n", line);
        free(line);
        X509_free(cert);
        return true;
    } else {
        //printf("No certificates.\n");
        return false;
    }
}

static int
process_job(xmlNode * a_node, double *share, int *user_id, bool co) {
    xmlNode *cur_node = NULL;
    //double a=0.001;
    //double a=1;



    for (cur_node = a_node; cur_node; cur_node = cur_node->next) {
        if (cur_node->type == XML_ELEMENT_NODE) {
            xmlChar *val = xmlNodeGetContent(cur_node);
            printf("node type: Element, name: %s, value: %s\n", cur_node->name, val);

            if ((!xmlStrcmp(cur_node->name, (const xmlChar *) "CompID"))) {
                int job_id;
                sscanf((char *) val, "%d", &job_id);
                if (co) {
                    if (job_id == 0) {
                        int rnum[1];


                        RAND_bytes((unsigned char *) &rnum[0], sizeof (rnum));
                        rnum[0] = abs(rnum[0]) % 100000;
                        cur_node = cur_node->next;
                        xmlChar *val1 = xmlNodeGetContent(cur_node);
                        //printf("Adding node type: Element, name: %s, value: %s\n", cur_node->name, val1);
                        std::lock_guard<std::mutex> guard(jobs_mutex);
                        //std::string s1((char *)val1);
                        jobs[rnum[0]][(char *) cur_node->name] = (char *) val1;


                        cur_node = cur_node->next;
                        xmlChar *val2 = xmlNodeGetContent(cur_node);
                        // printf("Adding node type: Element, name: %s, value: %s\n", (char *)cur_node->name, (char *)val2);

                        //std::string s2((char *)val2);
                        jobs[rnum[0]][(char *) cur_node->name] = (char *) val2;


                        cur_node = cur_node->next;
                        xmlChar *val3 = xmlNodeGetContent(cur_node);
                        // printf("Adding node type: Element, name: %s, value: %s\n", (char *)cur_node->name, (char *)val3);

                        //std::string s2((char *)val2);
                        jobs[rnum[0]][(char *) cur_node->name] = (char *) val3;


                        cur_node = cur_node->next;
                        xmlChar *val4 = xmlNodeGetContent(cur_node);
                        //printf("Adding node type: Element, name: %s, value: %s\n", (char *)cur_node->name, (char *)val4);

                        //std::string s2((char *)val2);
                        jobs[rnum[0]][(char *) cur_node->name] = (char *) val4;
                        //                    cur_node = cur_node->next;
                        //                    xmlChar *val3 = xmlNodeGetContent(cur_node);
                        //                    printf("Adding node type: Element, name: %s, value: %s\n", (char *)cur_node->name, (char *)val3);
                        //                    
                        //                    //std::string s2((char *)val2);
                        //                    jobs[rnum[0]][(char *)cur_node->name]=(char *)val3;


                        //printf("%s\n",jobs[rnum[0]]["DP"].c_str());
                        xmlFree(val);
                        xmlFree(val1);
                        xmlFree(val2);
                        xmlFree(val3);
                        xmlFree(val4);
                        return rnum[0];
                    } else {

                        if (jobs.find(job_id) == jobs.end()) {
                            std::lock_guard<std::mutex> guard2(shares_mutex);
                            shares.erase(job_id);
                            return 0; //we should do something else instead of killing the server if the job is not found...
                        }
                        
                        
                        
                        
                        double eps;
                        double sens;
                        double a;
                        int d;
                        sscanf(jobs[job_id]["DP"].c_str(), "%lf", &eps);



                        sscanf(jobs[job_id]["Wsize"].c_str(), "%d", &d);
                        sscanf(jobs[job_id]["a"].c_str(), "%lf", &a);
                        //sscanf(jobs[id]["Sensitivity"].c_str(),"%lf",&sens);

                        std::lock_guard<std::mutex> guard1(jobs_mutex);
                        jobs.erase(job_id);
                        
                        
                        //read the dropped users
                        cur_node = cur_node->next;
                        if(cur_node!=NULL){
                            xmlChar *val_droped = xmlNodeGetContent(cur_node);
                            printf("node type: Element, name: %s, value: %s\n", cur_node->name, val_droped);
                            if ((!xmlStrcmp(cur_node->name, (const xmlChar *) "Drop"))){
                                char * pch;
                                pch = strtok ((char *) val_droped,";");
                                while (pch != NULL)
                                {
                                    printf ("%s\n",pch);
                                    int u_id;
                                    sscanf(pch, "%d", &u_id);
                                    shares.at(job_id).erase(u_id);
                                    pch = strtok (NULL, ";");
                                }
                            }
                        }
                        
                        
                        double sum = 0.0;
                        int min = INT_MAX;
                        
                        for (auto& it: shares.at(job_id)) {
                            sum+=it.second.first;
                            if (min > it.second.second)
                                min = it.second.second;
                        }
                        
                        
//                        for (std::pair<double, int> n : shares.at(job_id)) {
//                            sum += n.first;
//                            if (min > n.second)
//                                min = n.second;
//                        }
                        sens = 2.0 / (a * min);
                        printf("EPS value: %lf\nSensitivity: %lf\nk: %d\nW_size: %d\nalpha: %lf\n", eps, sens, min, d, a);
                        if (eps > 0.00000001) {
                            for (int i = 0; i < d; i++) {
                                int rnum[1];
                                RAND_bytes((unsigned char *) &rnum[0], sizeof (rnum));
                                double rnd = ((double) rnum[0] / (double) (INT_MAX)) / 2.0;
                                //printf("Random value between -0.5 and 0.5: %lf",rnd);0;
                                //
                                //compute laplace noise
                                if (rnd < 0.0) {
                                    share[i] = sens / eps * log(1.0 - 2.0 * abs(rnd));
                                } else {
                                    share[i] = (-1.0) * sens / eps * log(1.0 - 2.0 * abs(rnd));
                                }
                                printf("Noisy value: %lf\n", share[i]);
                                share[i] *= 100000;
                            }
                        }

                        //if(shares[id].size()>1)
                        for (int i = 0; i < d; i++)
                            share[i] += (-1.0) * sum;
                        //else
                        //    for(int i=0;i<d;i++)
                        //        share[i]=0.0;

                        std::lock_guard<std::mutex> guard2(shares_mutex);
                        shares.erase(job_id);

                        return d; //this is a hack because even without erasing id, we cannot read Wsize outside this function for some reason...
                    }
                } else {
                    if (jobs.find(job_id) == jobs.end()) {
                        printf("Job not found: %d\n", job_id);
                        return 0;
                    }
                    int rnum[1];
                    RAND_bytes((unsigned char *) &rnum[0], sizeof (rnum));
                    //cur_node = cur_node->next;
                    //xmlChar *val1 = xmlNodeGetContent(cur_node);
                    //printf("Adding node type: Element, name: %s, value: %s\n", cur_node->name, val1);
                    //int sh;
                    //sscanf((char *)val1,"%ld",&cid);
                    //std::pair<int,bool> temp;
                    //temp.first=rnum[0];
                    //temp.second=false;
                    
                    cur_node = cur_node->next;
                        if(cur_node!=NULL){
                            xmlChar *val_u_id = xmlNodeGetContent(cur_node);
                            printf("node type: Element, name: %s, value: %s\n", cur_node->name, val_u_id);
                            if ((!xmlStrcmp(cur_node->name, (const xmlChar *) "User"))){
                                int u_id;
                                sscanf((char *) val_u_id, "%d", &u_id);
                                user_id[0]=u_id;
                            }
                        }
                    
                    

                    xmlFree(val);
                    //xmlFree(val1);
                    share[0] = rnum[0] % 100000;
                    return job_id;
                }
            }

        }

        //print_element_names(cur_node->children);
    }
    return -1;
}

int parsemsg(char* xml, int length, double *share, int *u_id, bool co) {
    xmlDocPtr doc;
    xmlNode *root_element = NULL;
    doc = xmlReadMemory(xml, length, "noname.xml", NULL, 0);
    if (doc == NULL) {
        fprintf(stderr, "Failed to parse document\n");
        return -1;
    } else {
        root_element = xmlDocGetRootElement(doc);
        int job_id = process_job(root_element->children, share, u_id, co);
        xmlFreeDoc(doc);
        return job_id;
    }

}

void* Servlet(void *arg) /* Serve the connection -- threadable */ {
    SSL *ssl = ((SSL *) arg);
    char buf[4096];
    char reply[4096];
    int sd, bytes;


    //printf("Thread id: %lu\n", pthread_self());
    if (SSL_accept(ssl) == FAIL) /* do SSL-protocol accept */
        ERR_print_errors_fp(stderr);
    else {
        if (ShowCerts(ssl)) { /* get any certificates */
            const char* echo;
            bytes = SSL_read(ssl, buf, sizeof (buf)); /* get request */
            if (bytes > 0) {
                buf[bytes] = 0;
                //printf("Coordinator msg: \"%s\"\n", buf);
                double share[10000];
                share[0] = -1.0;
                int u_id[1];
                u_id[0]=-1;

                // printf("Array 0: %lf\n", share[0]);
                int job_id = parsemsg(buf, bytes, share,u_id, true);
                char vOut [12];
                snprintf(vOut, sizeof (vOut), "%u", job_id);

                //printf("Array 0: %lf\n", share[0]);

                if (share[0] == -1.0) {
                    echo = "<Body><CompID>%s</CompID></Body>\n";
                    printf("Adding job %d\n", job_id);
                    shares[job_id];
                    sprintf(reply, echo, vOut);
                    SSL_write(ssl, reply, strlen(reply));
                } else {

                    //add DP if necessary, delete job





                    std::string total = "<Body><Shares>";
                    // printf("d: %d\n", d);
                    //here's the hack because we cannot read Wsize, so we used the job id to store it...
                    for (int i = 0; i < job_id; i++) {
                        echo = "<Share>%s</Share>";
                        char vOut2 [20];
                        snprintf(vOut2, sizeof (vOut2), "%lf", share[i]);
                        sprintf(reply, echo, vOut2);
                        
                        total += reply;

                    }
                    total += "</Shares></Body>\n";
                    //printf("%d\n",strlen(total.c_str()));
                    SSL_write(ssl, total.c_str(), strlen(total.c_str()));
                }

                //sprintf(reply, echo, vOut);
                //sprintf(reply, echo, buf);   /* construct reply */
                /* send reply */
            } else
                ERR_print_errors_fp(stderr);
        } else {
            const char* echo = "<Body><CompID>%s</CompID><Share>%s</Share><Epsilon>%s</Epsilon></Body>\n";
            bytes = SSL_read(ssl, buf, sizeof (buf)); /* get request */
            if (bytes > 0) {
                buf[bytes] = 0;
                //printf("Worker msg: \"%s\"\n", buf);
                double share[1];
                int u_id[1];
                share[0] = 0.0;
                u_id[0]=0;
                int job_id = parsemsg(buf, bytes, share, u_id, false);

                if (jobs.find(job_id) != jobs.end()) {


                    //int share=shares[id][cid].first;
                    char vOut1 [12];
                    snprintf(vOut1, sizeof (vOut1), "%u", job_id);
                    char vOut2 [20];
                    snprintf(vOut2, sizeof (vOut2), "%lf", share[0]);
                    printf("Sending share %lf for job %u to worker %d\n", share[0], job_id, u_id[0]);
                    
                    sprintf(reply, echo, vOut1, vOut2, jobs[job_id]["DP"].c_str()); /* construct reply */
                    SSL_write(ssl, reply, strlen(reply)); /* send reply */
                    //printf("Sent!");
                    buf[0] = 0;
                    bytes = SSL_read(ssl, buf, sizeof (buf)); /* get response */

                    if (bytes > 0) {
                        buf[bytes] = 0;
                        printf("Worker msg: \"%s\"\n", buf);
                        if (strcmp(buf, "OK") == 0 && jobs.find(job_id) != jobs.end()) {
                            buf[0] = 0;
                            bytes = SSL_read(ssl, buf, sizeof (buf));
                            buf[bytes] = 0;
                            int card;
                            sscanf(buf, "%d", &card);
                            printf("Worker msg: \"%s\"\n", buf);
                            std::pair<double, int> tmp;
                            tmp.first = share[0];
                            tmp.second = card;
                            std::lock_guard<std::mutex> guard(shares_mutex);
                            int key = u_id[0];
                            //shares.at(job_id).insert({key, tmp});
                            shares.at(job_id)[key]=tmp;
                            printf("Client %d Accepted\n",key);
                        } else
                            printf("Client Rejected or Timed-out\n");
                    }
                } else
                    printf("Job not found!\n");
            } else
                ERR_print_errors_fp(stderr);
        }
    }
    sd = SSL_get_fd(ssl); /* get socket connection */
    SSL_free(ssl); /* release SSL state */
    close(sd); /* close connection */
    return NULL;
}

int main() {
    SSL_CTX *ctx;
    int server;
    char portnum[] = "31337";

    char CertFile[] = "./myCA/cacert.pem";
    char KeyFile[] = "./myCA/private/cakey.pem";

    SSL_library_init();

    ctx = InitServerCTX(); /* initialize SSL */
    LoadCertificates(ctx, CertFile, KeyFile); /* load certs */
    server = OpenListener(atoi(portnum)); /* create server socket */
    pthread_t tid[60];
    int i = 0;
    printf("Server started\n");
    for (;;) {
        struct sockaddr_in addr;
        socklen_t len = sizeof (addr);
        SSL *ssl;

        int client = accept(server, (struct sockaddr*) &addr, &len); /* accept connection as usual */
        //printf("Connection: %s:%hu\n",inet_ntoa(addr.sin_addr), ntohs(addr.sin_port));
        ssl = SSL_new(ctx); /* get new SSL state with context */
        SSL_set_fd(ssl, client); /* set connection socket to SSL state */
        if (pthread_create(&tid[i++], NULL, Servlet, (void *) ssl) != 0)
            printf("Failed to create thread\n");
        if (i >= 50) {
            i = 0;
            while (i < 50) {
                pthread_join(tid[i++], NULL);
            }
            i = 0;
        }
        //Servlet(ssl);         /* service connection */
    }
    close(server); /* close server socket */
    SSL_CTX_free(ctx); /* release context */
}

