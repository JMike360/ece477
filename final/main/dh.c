#include <stdio.h>
#include <math.h>
#include "../include/bt.h"

#define COMMON_NUM 32719

// double diff_hellman(double,double,double,double,double,double,double,double,double);

int main(){
    double pc_B, pr_A, pr_B; // public keys , private keys, 
    //values above will be initialized

    double  kr_A,kr_B; //received keys;
    double publicVal_a, publicVal_b;// public values of a and b
    double expublicVal_a, expublicVal_b;// exchange value of public values of a and b

    double returnVal;
    pc_B =9; pr_A = 4; pr_B=3; kr_A=0;kr_B=0;
    publicVal_a = 0; publicVal_b = 0;
    expublicVal_a =0; expublicVal_b = 0;

    // returnVal = diff_hellman( COMMON_NUM, pc_B, pr_A, pr_B, kr_A, kr_B, publicVal_a, publicVal_b, expublicVal_a, expublicVal_b);
    return 0;
}

void diff_hellman_send(double public_key_to_send, double private_val) {
    int publicVal = (int)pow(public_key_to_send, private_val) % COMMON_NUM;
    char toSend[3] = {(publicVal & 0xff00) >> 8, (publicVal & 0x00ff) >> 0, '\n'};
}

int diff_hellman_recv(double exchanged_key, double private_val) {
    return (int)pow(exchanged_key, private_val) % COMMON_NUM;
}

/*
double diff_hellman(double pc_A, double pc_B, double pr_A, double pr_B, double kr_A, double kr_B, double publicVal_a, double publicVal_b, double expublicVal_a, double expublicVal_b){
    publicVal_a = fmod(pow(pc_B,pr_A) , pc_A);
    publicVal_b = fmod(pow(pc_B,pr_B) , pc_A);
    expublicVal_a = publicVal_b;
    expublicVal_b = publicVal_a;
    kr_A = fmod(pow(expublicVal_a,pr_A) , pc_A);
    kr_B = fmod(pow(expublicVal_b,pr_B) , pc_A);
    printf("%lf should be equal to %lf",kr_A,kr_B);
    return kr_A;
}
*/