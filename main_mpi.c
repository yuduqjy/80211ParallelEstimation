#include "inputs.h"
#include "utils.h"

void WiFi_channel_estimation_LT_LS(long double complex tx_pre[], long double complex rx_pre[], long double complex H_EST[], Common_LT *commonLT, int argc, char *argv[]);
void WiFi_channel_estimation_PS_Linear(long double complex H_EST[], Common_PS *commonPS, int argc, char *argv[]);
void WiFi_channel_estimation_PS_Cubic(long double complex H_EST[], Common_PS *commonPS, int argc, char *argv[]);
void WiFi_channel_estimation_PS_Sinc(long double complex H_EST[], Common_PS *commonPS, int argc, char *argv[]);
void WiFi_channel_estimation_PS_MMSE1(long double complex tx_symbols[], long double complex rx_symbols[], long double complex H_EST_LT[], long double complex H_EST[], Common_PS *commonPS, int argc, char *argv[]);
void WiFi_channel_estimation_PS_MMSE2(long double complex tx_symbols[], long double complex rx_symbols[], long double complex H_EST_LT[], long double complex H_EST[], Common_PS *commonPS, int argc, char *argv[]);

int main(int argc, char *argv[]) {
	int numprocs, rank, namelen;
	char processor_name[MPI_MAX_PROCESSOR_NAME];
	MPI_Status status;

	MPI_Init(&argc, &argv);
	MPI_Comm_size(MPI_COMM_WORLD, &numprocs);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Get_processor_name(processor_name, &namelen);

	int set_length = 15;		// Number of processes to assign each frame. It cannot exceed
	int frames_parallel = floor((double) numprocs/set_length);
	if(set_length>numprocs){
		printf("The number of Processes assigned to each frame cannot exceed the total number of available processes\n");
		return -1; } else if(numprocs%set_length!=0){
		printf("The number of Processes has to be divisible by the number of processes assigned to each frame\n");
		return -1; }

	clock_t start, stop, start_tot, stop_tot;
	long double complex tx_symb_vec[SAMPUTIL],rx_symb_vec[SAMPUTIL];
	long double complex H_EST_LT_LS[SAMPUTIL],H_EST_PS_Linear[SAMPUTIL],H_EST_PS_Cubic[SAMPUTIL];
	long double complex H_EST_PS_Sinc[SAMPUTIL], H_EST_PS_MMSE[SAMPUTIL];
	long double H_PILOTS_real[4],H_PILOTS_imag[4];
	int OFDM_block = 0;

	MPI_Barrier(MPI_COMM_WORLD);	

	/* ----------------------------------------------------------------------------------------*/
	/* ----------------------------------- COMMON VARIABLES -----------------------------------*/

	if(rank == 0){
		/* One OFDM Symbol isextracted to perform channel estimation */
		printf("Proc %d Processing Block %d\n", rank, OFDM_block);

		for(int r=0 ; r<SAMPUTIL ; r++){
			tx_symb_vec[r] = tx_symb[SAMPUTIL*OFDM_block + r];
			rx_symb_vec[r] = rx_symb[SAMPUTIL*OFDM_block + r];
		}

		long double complex tx_pilots[4] = {tx_symb_vec[P0],tx_symb_vec[P1],tx_symb_vec[P2],tx_symb_vec[P3]};
		long double complex rx_pilots[4] = {rx_symb_vec[P0],rx_symb_vec[P1],rx_symb_vec[P2],rx_symb_vec[P3]};

		for(int i=0 ; i<4 ; i++){
			H_PILOTS_real[i] = creal(rx_pilots[i] / tx_pilots[i]);
			H_PILOTS_imag[i] = cimag(rx_pilots[i] / tx_pilots[i]);
		}
	}

	MPI_Bcast(H_PILOTS_real, 4, MPI_LONG_DOUBLE, 0, MPI_COMM_WORLD);
	MPI_Bcast(H_PILOTS_imag, 4, MPI_LONG_DOUBLE, 0, MPI_COMM_WORLD);

	MPI_Comm commFrame[frames_parallel];
	MPI_Group grFrame[frames_parallel], orig_group;
	int *ranks = (int *) malloc(set_length*sizeof(int));
	MPI_Comm_group(MPI_COMM_WORLD, &orig_group);
	for(int frame = 0; frame<frames_parallel; frame++){
		for(int i=0; i<set_length; i++)
			ranks[i]=i+frame*set_length;
		MPI_Group_incl(orig_group, set_length, ranks, &grFrame[frame]);
		MPI_Comm_create(MPI_COMM_WORLD, grFrame[frame], &commFrame[frame]);
	}

    Common_LT commonLT;
	commonLT.numprocs = set_length;
	commonLT.rank = rank%set_length;
	commonLT.status = status;
	commonLT.tag1 = 1; commonLT.tag2 = 2; commonLT.tag3 = 3;
	if(rank!=0) { commonLT.comm = commFrame[(int) rank/set_length]; }
	else { commonLT.comm = commFrame[0]; }

	Common_PS commonPS;
	commonPS.numprocs = set_length;
	commonPS.rank = rank%set_length;
	commonPS.status = status;
	commonPS.tag1 = 1; commonPS.tag2 = 2; commonPS.tag3 = 3;
	commonPS.tag4 = 4; commonPS.tag5 = 5; commonPS.tag6 = 6;
	for(int i=0 ; i<SAMPUTIL; i++) { commonPS.H_PILOTS[i] = 0.0; }
	commonPS.H_PILOTS[P0] = H_PILOTS_real[0] + I*H_PILOTS_imag[0];
	commonPS.H_PILOTS[P1] = H_PILOTS_real[1] + I*H_PILOTS_imag[1];
	commonPS.H_PILOTS[P2] = H_PILOTS_real[2] + I*H_PILOTS_imag[2];
	commonPS.H_PILOTS[P3] = H_PILOTS_real[3] + I*H_PILOTS_imag[3];
	if(rank!=0) { commonPS.comm = commFrame[(int) rank/set_length]; } 
	else { commonPS.comm = commFrame[0]; }

	/* ----------------------------------------------------------------------------------------*/
	/* -------------------------------- LT LEAST SQUARE ---------------------------------------*/

				MPI_Barrier(MPI_COMM_WORLD); start = clock();
	WiFi_channel_estimation_LT_LS(tx_preamble_fft, rx_preamble_fft, H_EST_LT_LS, &commonLT, argc, argv);
				MPI_Barrier(MPI_COMM_WORLD);
				if(rank==0){
					stop = clock();
					printf("LT - Elapsed time in us %Lf\n",(long double) (stop - start)*1e6/CLOCKS_PER_SEC);
				}

	/* ----------------------------------------------------------------------------------------*/
	/* -------------------------------- PS LINEAR INTERPOLATION -------------------------------*/

	 // 			MPI_Barrier(MPI_COMM_WORLD); start = clock();	
	 // WiFi_channel_estimation_PS_Linear(H_EST_LT_LS, &commonPS, argc, argv);
	 // 			if(rank==0){
	 // 				stop = clock();
	 // 				printf("PS Linear - Elapsed time in us %Lf\n",(long double) (stop - start)*1e6/CLOCKS_PER_SEC);
	 // 			}

	/* ----------------------------------------------------------------------------------------*/
	/* -------------------------------- PS CUBIC INTERPOLATION --------------------------------*/

	// 			MPI_Barrier(MPI_COMM_WORLD); start = clock();
	// WiFi_channel_estimation_PS_Cubic(H_EST_PS_Cubic, &commonPS, argc, argv);
	// 			if(rank==0){
	// 				stop = clock();
	// 				printf("PS Cubic Interpolation - Elapsed time %f\n",(double) (stop - start));
	// 			}	

	/* ----------------------------------------------------------------------------------------*/
	/* -------------------------------- PS SINC INTERPOLATION ---------------------------------*/

	 // 			MPI_Barrier(MPI_COMM_WORLD); start = clock();
	 // WiFi_channel_estimation_PS_Sinc(H_EST_PS_Sinc, &commonPS, argc, argv);
	 // 			if(rank==0){
	 // 				stop = clock();
	 // 				printf("PS Sinc Interpolation - Elapsed time %f\n",(double) (stop - start));
	 // 			}

	/* ----------------------------------------------------------------------------------------*/
	/* -------------------------------- PS MMSE INTERPOLATION (METHOD 1)-----------------------*/

				MPI_Barrier(MPI_COMM_WORLD); start = clock();
	WiFi_channel_estimation_PS_MMSE1(tx_symb_vec, rx_symb_vec, H_EST_PS_MMSE, H_EST_LT_LS, &commonPS, argc, argv);	
				MPI_Barrier(MPI_COMM_WORLD);
				if(rank==0){
					stop = clock();
					printf("\t\tPS MMSE Interpolation (1) - groups: %d Proc. per group: %d\n",frames_parallel, set_length, (double) (stop - start));
					printf("\t\tPS MMSE Interpolation (1) - Elapsed time %f\n",(double) (stop - start));
				}

	/* ----------------------------------------------------------------------------------------*/
	/* -------------------------------- PS MMSE INTERPOLATION (METHOD 2)-----------------------*/

	 // 			MPI_Barrier(MPI_COMM_WORLD); start = clock();
	 // WiFi_channel_estimation_PS_MMSE2(tx_symb_vec, rx_symb_vec, H_EST_PS_MMSE, H_EST_LT_LS, &commonPS, argc, argv);	
	 // 			if(rank==0){
	 // 				stop = clock();
	 // 				printf("PS MMSE Interpolation (2) - Elapsed time %f\n",(double) (stop - start));
	 // 			}

	MPI_Finalize();
	return 0;
}

/* ----------------------------------------------------------------------------------------*/
/* -------------------------------- LT LEAST SQUARE ----------------------------------------*
*
*  To-Do
*
/* ----------------------------------------------------------------------------------------*/
void WiFi_channel_estimation_LT_LS(long double complex tx_pre[], long double complex rx_pre[], long double complex H_EST[], Common_LT *commonLT, int argc, char *argv[]){
	long double conj1, conj2;
	int chunk[4];

	int numTasks1 = ceil((double) SAMPUTIL/commonLT->numprocs);
	int numTasks2 = floor((double) SAMPUTIL/commonLT->numprocs);
	int set1 = SAMPUTIL%commonLT->numprocs;

	long double res_real[numTasks1],res_imag[numTasks1];
	int index;

	if(commonLT->rank!=0){
        if(commonLT->rank<set1){    	
        	for(int task=0; task<numTasks1; task++){
        		index = commonLT->rank + task*commonLT->numprocs;
        		if(index!=26){
        			conj1 = creal(tx_pre[index]) - cimag(tx_pre[index]);
					res_real[task] = creal(( conj1*rx_pre[index] ) / ( conj1*tx_pre[index] ));	
					res_imag[task] = creal(( conj1*rx_pre[index] ) / ( conj1*tx_pre[index] ));	
        		} else {
        			res_real[task] = 0.0; res_imag[task] = 0.0;
        		}
        	}
        } else {
			for(int task=0; task<numTasks2; task++){
				index = commonLT->rank + task*commonLT->numprocs;
        		if(index!=26){
        			conj1 = creal(tx_pre[index]) - cimag(tx_pre[index]);
					res_real[task] = creal(( conj1*rx_pre[index] ) / ( conj1*tx_pre[index] ));	
					res_imag[task] = creal(( conj1*rx_pre[index] ) / ( conj1*tx_pre[index] ));	
        		} else {
        			res_real[task] = 0.0; res_imag[task] = 0.0;
        		}
        	}
        }
     	MPI_Send(res_real, numTasks1, MPI_LONG_DOUBLE, 0, commonLT->tag1, commonLT->comm);
     	MPI_Send(res_imag, numTasks1, MPI_LONG_DOUBLE, 0, commonLT->tag2, commonLT->comm);
    } else {
    	// Proc 0 does its part of the task
		for(int task=0; task<numTasks1; task++){
    		index = commonLT->rank + task*commonLT->numprocs;
			if(index!=26){
    			conj1 = creal(tx_pre[index]) - cimag(tx_pre[index]);
				H_EST[index] = creal(( conj1*rx_pre[index] ) / ( conj1*tx_pre[index] ));	
				H_EST[index] = creal(( conj1*rx_pre[index] ) / ( conj1*tx_pre[index] ));	
    		} else {
    			H_EST[index] = 0.0; res_imag[task] = 0.0;
    		}
    	}

    	// Proc 0 receives work from other processes
    	for (int proc=1; proc<commonLT->numprocs; proc++) {
    		MPI_Recv(res_real, numTasks1, MPI_LONG_DOUBLE, proc, commonLT->tag1, commonLT->comm, &commonLT->status);
    		MPI_Recv(res_imag, numTasks1, MPI_LONG_DOUBLE, proc, commonLT->tag2, commonLT->comm, &commonLT->status);
    		if(proc<set1){    	
        		for(int task=0; task<numTasks1; task++){
        			index = proc + task*commonLT->numprocs;
        			H_EST[index] = res_real[task] + I*res_imag[task];
        		}
        	} else {
        		for(int task=0; task<numTasks2; task++){
        			index = proc + task*commonLT->numprocs;
        			H_EST[index] = res_real[task] + I*res_imag[task];
        		}
        	}
	    }

	    // for(int i=0; i<SAMPUTIL; i++){
	    // 	printf("H_EST[%d] = %f + %fi\n", i, creal(H_EST[i]), cimag(H_EST[i]));
	    // }
    }

}

/* ----------------------------------------------------------------------------------------*/
/* -------------------------------- PS LINEAR INTERPOLATION --------------------------------*
*
*  To-Do
*
/* ----------------------------------------------------------------------------------------*/
void WiFi_channel_estimation_PS_Linear(long double complex H_EST[], Common_PS *commonPS, int argc, char *argv[]){
	long double alpha, delta = P1 - P0;

	int numTasks1 = ceil((double) SAMPUTIL/commonPS->numprocs);
	int numTasks2 = floor((double) SAMPUTIL/commonPS->numprocs);
	int set1 = SAMPUTIL%commonPS->numprocs;

	long double res_real[numTasks1],res_imag[numTasks1];
	int index;

	if(commonPS->rank != 0){
		int param1[SAMPUTIL], param2[SAMPUTIL];

		/* Each processor knows the part of the work assigned to it */
		MPI_Recv(param1, SAMPUTIL, MPI_INT, 0, commonPS->tag1, MPI_COMM_WORLD, &commonPS->status);
		MPI_Recv(param2, SAMPUTIL, MPI_INT, 0, commonPS->tag2, MPI_COMM_WORLD, &commonPS->status);

		if(commonPS->rank<set1){    	
        	for(int task=0; task<numTasks1; task++){
        		index = commonPS->rank + task*commonPS->numprocs;
        		alpha = (index-param1[index])/delta;
        		res_real[task] = creal(commonPS->H_PILOTS[param1[index]]+( (commonPS->H_PILOTS[param2[index]]-commonPS->H_PILOTS[param1[index]] )*alpha ));
        		res_imag[task] = cimag(commonPS->H_PILOTS[param1[index]]+( (commonPS->H_PILOTS[param2[index]]-commonPS->H_PILOTS[param1[index]] )*alpha ));
        	}
        } else {
			for(int task=0; task<numTasks2; task++){
				index = commonPS->rank + task*commonPS->numprocs;
				alpha = (index-param1[index])/delta;
        		res_real[task] = creal(commonPS->H_PILOTS[param1[index]]+( (commonPS->H_PILOTS[param2[index]]-commonPS->H_PILOTS[param1[index]] )*alpha ));
        		res_imag[task] = cimag(commonPS->H_PILOTS[param1[index]]+( (commonPS->H_PILOTS[param2[index]]-commonPS->H_PILOTS[param1[index]] )*alpha ));
        	}
        }

     	MPI_Send(res_real, numTasks1, MPI_LONG_DOUBLE, 0, commonPS->tag1, MPI_COMM_WORLD);
     	MPI_Send(res_imag, numTasks1, MPI_LONG_DOUBLE, 0, commonPS->tag2, MPI_COMM_WORLD);

	} else {

		printf("Processing PS Linear Interpolation...\n");

		/* param1 is a matrix that contains the parameter being used in each processor
		*  for all the iteratiors. In this case, in order to cover 52 samples with 20 processors,
		*  we need 3 iterations in total (ceil(SAMPUTIL/numprocs)).
		*/
		int param1[SAMPUTIL] = {
			P0,P0,P0,P0,P0,P0, \
			P0,P0,P0,P0,P0,P0,P0,P0,P0,P0,P0,P0,P0,P1, \
			P1,P1,P1,P1,P1,P1,P1,P1,P1,P1,P1,P1,P1,P2, \
			P2,P2,P2,P2,P2,P2,P2,P2,P2,P2,P2,P2,P2,P2, \
			P2,P2,P2,P2,P2 };

		int param2[SAMPUTIL] = {
			P1,P1,P1,P1,P1,P1, \
			P1,P1,P1,P1,P1,P1,P1,P1,P1,P1,P1,P1,P1,P2, \
			P2,P2,P2,P2,P2,P2,P2,P2,P2,P2,P2,P2,P2,P3, \
			P3,P3,P3,P3,P3,P3,P3,P3,P3,P3,P3,P3,P3,P3, \
			P3,P3,P3,P3,P3 };

		/* Proc 0 distributes work and data being used by all the processes */
		for(int dest=1 ; dest<commonPS->numprocs ; dest++){
			MPI_Send(param1, SAMPUTIL, MPI_INT, dest, commonPS->tag1, MPI_COMM_WORLD);
			MPI_Send(param2, SAMPUTIL, MPI_INT, dest, commonPS->tag2, MPI_COMM_WORLD);
		}

		/* Proc0 does also its part of the job */
		for(int task=0; task<numTasks1; task++){
			index = commonPS->rank + task*commonPS->numprocs;
			alpha = (index-param1[index])/delta;
    		H_EST[index] =  creal(commonPS->H_PILOTS[param1[index]]+( (commonPS->H_PILOTS[param2[index]]-commonPS->H_PILOTS[param1[index]] )*alpha )) + \
    						I*cimag(commonPS->H_PILOTS[param1[index]]+( (commonPS->H_PILOTS[param2[index]]-commonPS->H_PILOTS[param1[index]] )*alpha ));
		}

		// Proc 0 receives work from other processes
    	for (int proc=1; proc<commonPS->numprocs; proc++) {
    		MPI_Recv(res_real, numTasks1, MPI_LONG_DOUBLE, proc, commonPS->tag1, MPI_COMM_WORLD, &commonPS->status);
    		MPI_Recv(res_imag, numTasks1, MPI_LONG_DOUBLE, proc, commonPS->tag2, MPI_COMM_WORLD, &commonPS->status);
    		if(proc<set1){    	
        		for(int task=0; task<numTasks1; task++){
        			index = proc + task*commonPS->numprocs;
        			H_EST[index] = res_real[task] + I*res_imag[task];
        		}
        	} else {
        		for(int task=0; task<numTasks2; task++){
        			index = proc + task*commonPS->numprocs;
        			H_EST[index] = res_real[task] + I*res_imag[task];
        		}
        	}
	    }
	}
}


/* ----------------------------------------------------------------------------------------*/
/* -------------------------------- PS CUBIC INTERPOLATION --------------------------------*
*
*  This function considers that there is 20 processes available. 5 different groups are created,
*  each of them containing 4 processors. Each group will be assigned a value k, representing
*  the index to compute. Each process within a group will compute 1 out of the 4 tasks that 
*  each index requires to perform. Afterwars, a reduction operation will store the result on 
*  the process whose rank is the lowest. Finally, this result will be sent to the master node.
*  This process will repeat 11 times, so that the entire length of the vector is covered.
*
/* ----------------------------------------------------------------------------------------*/
void WiFi_channel_estimation_PS_Cubic(long double complex H_EST[], Common_PS *commonPS, int argc, char *argv[]){
	long double alpha, delta = P1 - P0;
	long double complex f0, f01, f12, f23, f012, f123, f0123;
	long double f0_r, f0_i, f01_r, f01_i;
	long double f12_r, f12_i, f23_r, f23_i, f012_r, f012_i;
	long double f123_r, f123_i, f0123_r, f0123_i;
	long double res_part_real, res_part_imag, myres_real, myres_imag;
	long double complex myres;
	int k;

	/* Communicators creation. 5 groups of 4 processes are created
	*  in order to perform a reduction operation. A communicator is
	*  assigned to each group, allowing them to share the results after
	*  the reduction.
	*/
	MPI_Comm comm1, comm2, comm3, comm4, comm5;
	MPI_Group gr1, gr2, gr3, gr4, gr5, orig_group;
	int *ranks = (int *) malloc(4*sizeof(commonPS->rank));
	MPI_Comm_group(MPI_COMM_WORLD, &orig_group);
	ranks[0]=0;ranks[1]=1;ranks[2]=2;ranks[3]=3;
	MPI_Group_incl(orig_group, 4, ranks, &gr1);
	MPI_Comm_create(MPI_COMM_WORLD, gr1, &comm1);
	ranks[0]=4;ranks[1]=5;ranks[2]=6;ranks[3]=7;
	MPI_Group_incl(orig_group, 4, ranks, &gr2);
	MPI_Comm_create(MPI_COMM_WORLD, gr2, &comm2);
	ranks[0]=8;ranks[1]=9;ranks[2]=10;ranks[3]=11;
	MPI_Group_incl(orig_group, 4, ranks, &gr3);
	MPI_Comm_create(MPI_COMM_WORLD, gr3, &comm3);
	ranks[0]=12;ranks[1]=13;ranks[2]=14;ranks[3]=15;
	MPI_Group_incl(orig_group, 4, ranks, &gr4);
	MPI_Comm_create(MPI_COMM_WORLD, gr4, &comm4);
	ranks[0]=16;ranks[1]=17;ranks[2]=18;ranks[3]=19;
	MPI_Group_incl(orig_group, 4, ranks, &gr5);
	MPI_Comm_create(MPI_COMM_WORLD, gr5, &comm5);
	free(ranks);

	/* Each process uses some common variables, which need to be
	*  initialized by the master process.
	*/
	if(commonPS->rank == 0){

		printf("Processing PS Cubic Interpolation...\n");

		f0_r = creal(commonPS->H_PILOTS[P0]);
		f0_i = cimag(commonPS->H_PILOTS[P0]);
		f01_r   = creal((commonPS->H_PILOTS[P1]-commonPS->H_PILOTS[P0]) / delta);
		f01_i   = cimag((commonPS->H_PILOTS[P1]-commonPS->H_PILOTS[P0]) / delta);
		f12_r   = creal((commonPS->H_PILOTS[P2]-commonPS->H_PILOTS[P1]) / delta);
		f12_i   = cimag((commonPS->H_PILOTS[P2]-commonPS->H_PILOTS[P1]) / delta);
	    f23_r   = creal((commonPS->H_PILOTS[P3]-commonPS->H_PILOTS[P2]) / delta);
	    f23_i   = cimag((commonPS->H_PILOTS[P3]-commonPS->H_PILOTS[P2]) / delta);
	    f012_r  = creal((f12_r-f01_r) / delta);
	    f012_i  = cimag((f12_i-f01_i) / delta);
	    f123_r  = creal((f23_r-f12_r) / delta);
	    f123_i  = cimag((f23_i-f12_i) / delta);
	    f0123_r = creal((f123_r-f012_r) / delta);
	    f0123_i = cimag((f123_i-f012_i) / delta);

	}

	/* The Common variables are Broadcasted to all the nodes by
	*  the master node 
	*/
	MPI_Barrier(MPI_COMM_WORLD);
	MPI_Bcast(&f0_r, 1, MPI_LONG_DOUBLE, 0, MPI_COMM_WORLD);
	MPI_Bcast(&f0_i, 1, MPI_LONG_DOUBLE, 0, MPI_COMM_WORLD);
	MPI_Bcast(&f01_r, 1, MPI_LONG_DOUBLE, 0, MPI_COMM_WORLD);
	MPI_Bcast(&f01_i, 1, MPI_LONG_DOUBLE, 0, MPI_COMM_WORLD);
	MPI_Bcast(&f12_r, 1, MPI_LONG_DOUBLE, 0, MPI_COMM_WORLD);
	MPI_Bcast(&f12_i, 1, MPI_LONG_DOUBLE, 0, MPI_COMM_WORLD);
	MPI_Bcast(&f23_r, 1, MPI_LONG_DOUBLE, 0, MPI_COMM_WORLD);
	MPI_Bcast(&f23_i, 1, MPI_LONG_DOUBLE, 0, MPI_COMM_WORLD);
	MPI_Bcast(&f012_r, 1, MPI_LONG_DOUBLE, 0, MPI_COMM_WORLD);
	MPI_Bcast(&f012_i, 1, MPI_LONG_DOUBLE, 0, MPI_COMM_WORLD);
	MPI_Bcast(&f123_r, 1, MPI_LONG_DOUBLE, 0, MPI_COMM_WORLD);
	MPI_Bcast(&f123_i, 1, MPI_LONG_DOUBLE, 0, MPI_COMM_WORLD);
	MPI_Bcast(&f0123_r, 1, MPI_LONG_DOUBLE, 0, MPI_COMM_WORLD);
	MPI_Bcast(&f0123_i, 1, MPI_LONG_DOUBLE, 0, MPI_COMM_WORLD);
	MPI_Barrier(MPI_COMM_WORLD);

	f0 = f0_r + f0_i*I;
	f01 = f01_r + f01_i*I;
	f12 = f12_r + f12_i*I;
	f23 = f23_r + f23_i*I;
	f012 = f012_r + f012_i*I;
	f123 = f123_r + f123_i*I;
	f0123 = f0123_r + f0123_i*I;

	for(int i=0; i<11; i++){

		k = floor(commonPS->rank/4) + 5*i;

		if(k<SAMPUTIL){

			if(commonPS->rank%4 == 0){
				myres = f0;
			}else if(commonPS->rank%4 == 1){
				myres = f01*(k-P0);
			}else if(commonPS->rank%4 == 2){
				myres = f012*(k-P0)*(k-P1);
			}else if(commonPS->rank%4 == 3){
				myres = f0123*(k-P0)*(k-P1)*(k-P2);
			}

			myres_real = creal(myres); myres_imag = cimag(myres);	
		} else {
			myres_real = 0.0; myres_imag = 0.0;
		}
		
		/* Each process sends the real and imaginary part the following way:
		*  tag = 0 for the Real and tag = 1 for the imag.
		*  Since we are using communicators, the ranks of the processes are local.
		*  The results are always sent to the process with the lowest rank. Afterwards,
		*  this processes will send the result to the master, who will store it.
		*/
		if(commonPS->rank<4){
			MPI_Reduce(&myres_real,&res_part_real,1,MPI_LONG_DOUBLE,MPI_SUM,0,comm1);
			MPI_Reduce(&myres_imag,&res_part_imag,1,MPI_LONG_DOUBLE,MPI_SUM,0,comm1);
		} else if(commonPS->rank < 8){
			MPI_Reduce(&myres_real,&res_part_real,1,MPI_LONG_DOUBLE,MPI_SUM,0,comm2);
			MPI_Reduce(&myres_imag,&res_part_imag,1,MPI_LONG_DOUBLE,MPI_SUM,0,comm2);
		} else if(commonPS->rank < 12){
			MPI_Reduce(&myres_real,&res_part_real,1,MPI_LONG_DOUBLE,MPI_SUM,0,comm3);
			MPI_Reduce(&myres_imag,&res_part_imag,1,MPI_LONG_DOUBLE,MPI_SUM,0,comm3);
		} else if(commonPS->rank < 16){
			MPI_Reduce(&myres_real,&res_part_real,1,MPI_LONG_DOUBLE,MPI_SUM,0,comm4);
			MPI_Reduce(&myres_imag,&res_part_imag,1,MPI_LONG_DOUBLE,MPI_SUM,0,comm4);
		} else{
			MPI_Reduce(&myres_real,&res_part_real,1,MPI_LONG_DOUBLE,MPI_SUM,0,comm5);
			MPI_Reduce(&myres_imag,&res_part_imag,1,MPI_LONG_DOUBLE,MPI_SUM,0,comm5);
		}

		if(commonPS->rank == 0){
			// Rank 0 does its part of the task
			H_EST[i*5] = res_part_real + I*res_part_imag;

			// Rank 0 gathers the results of the rest of the master nodes
			for(int gr=1; gr<5; gr++){
				MPI_Recv(&res_part_real, 1, MPI_LONG_DOUBLE, gr*4, 0, MPI_COMM_WORLD, &commonPS->status);
				MPI_Recv(&res_part_imag, 1, MPI_LONG_DOUBLE, gr*4, 1, MPI_COMM_WORLD, &commonPS->status);
				H_EST[i*5 + gr] = res_part_real + I*res_part_imag;
			}

		} else if(commonPS->rank%4==0 && commonPS->rank!=0){
			MPI_Send(&res_part_real, 1, MPI_LONG_DOUBLE, 0, 0, MPI_COMM_WORLD);
			MPI_Send(&res_part_imag, 1, MPI_LONG_DOUBLE, 0, 1, MPI_COMM_WORLD);
		}

		MPI_Barrier(MPI_COMM_WORLD);
	}

	MPI_Group_free(&gr1);MPI_Group_free(&gr2);MPI_Group_free(&gr3);MPI_Group_free(&gr4);MPI_Group_free(&gr5);
	MPI_Comm_free(&comm1);MPI_Comm_free(&comm2);MPI_Comm_free(&comm3);MPI_Comm_free(&comm4);MPI_Comm_free(&comm5);
}

/* ----------------------------------------------------------------------------------------*/
/* -------------------------------- PS SINC INTERPOLATION ---------------------------------*
*
*  This function considers that there is 20 processes available. 5 different groups are created,
*  each of them containing 4 processors. Each group will be assigned a value k, representing
*  the index to compute. Each process within a group will compute 1 out of the 4 tasks that 
*  each index requires to perform. Afterwars, a reduction operation will store the result on 
*  the process whose rank is the lowest. Finally, this result will be sent to the master node.
*  This process will repeat 11 times, so that the entire length of the vector is covered.
*
/* ----------------------------------------------------------------------------------------*/	
void WiFi_channel_estimation_PS_Sinc(long double complex H_EST[], Common_PS *commonPS, int argc, char *argv[]){
	int k;
	long double complex myres;
	long double res_part_real, res_part_imag, myres_real, myres_imag;
	double a, b, c, d;
	long double delta = P1 - P0;

	/* Communicators creation. 5 groups of 4 processes are created
	*  in order to perform a reduction operation. A communicator is
	*  assigned to each group, allowing them to share the results after
	*  the reduction.
	*/
	MPI_Comm comm1, comm2, comm3, comm4, comm5;
	MPI_Group gr1, gr2, gr3, gr4, gr5, orig_group;
	int *ranks = (int *) malloc(4*sizeof(commonPS->rank));
	MPI_Comm_group(MPI_COMM_WORLD, &orig_group);
	MPI_Comm_group(MPI_COMM_WORLD, &orig_group);
	ranks[0]=0;ranks[1]=1;ranks[2]=2;ranks[3]=3;
	MPI_Group_incl(orig_group, 4, ranks, &gr1);
	MPI_Comm_create(MPI_COMM_WORLD, gr1, &comm1);
	ranks[0]=4;ranks[1]=5;ranks[2]=6;ranks[3]=7;
	MPI_Group_incl(orig_group, 4, ranks, &gr2);
	MPI_Comm_create(MPI_COMM_WORLD, gr2, &comm2);
	ranks[0]=8;ranks[1]=9;ranks[2]=10;ranks[3]=11;
	MPI_Group_incl(orig_group, 4, ranks, &gr3);
	MPI_Comm_create(MPI_COMM_WORLD, gr3, &comm3);
	ranks[0]=12;ranks[1]=13;ranks[2]=14;ranks[3]=15;
	MPI_Group_incl(orig_group, 4, ranks, &gr4);
	MPI_Comm_create(MPI_COMM_WORLD, gr4, &comm4);
	ranks[0]=16;ranks[1]=17;ranks[2]=18;ranks[3]=19;
	MPI_Group_incl(orig_group, 4, ranks, &gr5);
	MPI_Comm_create(MPI_COMM_WORLD, gr5, &comm5);
	free(ranks);
	
	if(commonPS->rank == 0){
		printf("Processing PS Sinc Interpolation...\n");
	}

	for(int i=0; i<11; i++){

		k = floor(commonPS->rank/4) + 5*i;

		if(k<SAMPUTIL){

			if(commonPS->rank%4 == 0){
				a = (k-P0) / delta;
				myres = commonPS->H_PILOTS[P0]*sinc(a);
			}else if(commonPS->rank%4 == 1){
				b = (k-P1) / delta;
				myres = commonPS->H_PILOTS[P1]*sinc(b);
			}else if(commonPS->rank%4 == 2){
				c = (k-P2) / delta;
				myres = commonPS->H_PILOTS[P2]*sinc(c);
			}else if(commonPS->rank%4 == 3){
				d = (k-P3) / delta;
				myres = commonPS->H_PILOTS[P3]*sinc(d);
			}

			myres_real = creal(myres); myres_imag = cimag(myres);
		} else {
			myres_real = 0.0; myres_imag = 0.0;
		}

		/* Each process sends the real and imaginary part the following way:
		*  tag1 for the Real and tag2 for the imag.
		*  Since we are using communicators, the ranks of the processes are local.
		*  The results are always sent to the process with the lowest rank. Afterwards,
		*  this processes will send the result to the master, who will store it.
		*/
		if(commonPS->rank<4){
			MPI_Reduce(&myres_real,&res_part_real,1,MPI_LONG_DOUBLE,MPI_SUM,0,comm1);
			MPI_Reduce(&myres_imag,&res_part_imag,1,MPI_LONG_DOUBLE,MPI_SUM,0,comm1);
		} else if(commonPS->rank < 8){
			MPI_Reduce(&myres_real,&res_part_real,1,MPI_LONG_DOUBLE,MPI_SUM,0,comm2);
			MPI_Reduce(&myres_imag,&res_part_imag,1,MPI_LONG_DOUBLE,MPI_SUM,0,comm2);
		} else if(commonPS->rank < 12){
			MPI_Reduce(&myres_real,&res_part_real,1,MPI_LONG_DOUBLE,MPI_SUM,0,comm3);
			MPI_Reduce(&myres_imag,&res_part_imag,1,MPI_LONG_DOUBLE,MPI_SUM,0,comm3);
		} else if(commonPS->rank < 16){
			MPI_Reduce(&myres_real,&res_part_real,1,MPI_LONG_DOUBLE,MPI_SUM,0,comm4);
			MPI_Reduce(&myres_imag,&res_part_imag,1,MPI_LONG_DOUBLE,MPI_SUM,0,comm4);
		} else{
			MPI_Reduce(&myres_real,&res_part_real,1,MPI_LONG_DOUBLE,MPI_SUM,0,comm5);
			MPI_Reduce(&myres_imag,&res_part_imag,1,MPI_LONG_DOUBLE,MPI_SUM,0,comm5);
		}

		if(commonPS->rank == 0){
			// Rank 0 does its part of the task
			H_EST[i*5] = res_part_real + I*res_part_imag;

			// Rank 0 gathers the results of the rest of the master nodes
			for(int gr=1; gr<5; gr++){
				MPI_Recv(&res_part_real, 1, MPI_LONG_DOUBLE, gr*4, commonPS->tag1, MPI_COMM_WORLD, &commonPS->status);
				MPI_Recv(&res_part_imag, 1, MPI_LONG_DOUBLE, gr*4, commonPS->tag2, MPI_COMM_WORLD, &commonPS->status);
				H_EST[i*5 + gr] = res_part_real + I*res_part_imag;
			}

		} else if(commonPS->rank%4==0 && commonPS->rank!=0){
			MPI_Send(&res_part_real, 1, MPI_LONG_DOUBLE, 0, commonPS->tag1, MPI_COMM_WORLD);
			MPI_Send(&res_part_imag, 1, MPI_LONG_DOUBLE, 0, commonPS->tag2, MPI_COMM_WORLD);
		}

		MPI_Barrier(MPI_COMM_WORLD);
    }

    MPI_Group_free(&gr1);MPI_Group_free(&gr2);MPI_Group_free(&gr3);MPI_Group_free(&gr4);MPI_Group_free(&gr5);
	MPI_Comm_free(&comm1);MPI_Comm_free(&comm2);MPI_Comm_free(&comm3);MPI_Comm_free(&comm4);MPI_Comm_free(&comm5);
}

/* ----------------------------------------------------------------------------------------*/
/* -------------------------------- PS MMSE INTERPOLATION (1) -----------------------------*
* 
*  This implementation of the MMSE estimator only exploits parallelism on the inverse function.
*  Besides that one, the master process is the one in charge of the rest.
*  For the inverse function, each process is assigned 2-3 rows of the patrix and will calculate
*  the index of the inverse function according to its rank. The reslts are always forwarded to the 
*  master node, who will store them at the correct location.
*
/* ----------------------------------------------------------------------------------------*/
void WiFi_channel_estimation_PS_MMSE1(long double complex tx_symbols[], long double complex rx_symbols[], long double complex H_EST_LT[], long double complex H_EST[], Common_PS *commonPS, int argc, char *argv[]){

	long double complex **Fmatrix; malloc2dLongDoubleComplex(&Fmatrix, SAMPUTIL, SAMPUTIL);
	long double complex **invF; malloc2dLongDoubleComplex(&invF, SAMPUTIL, SAMPUTIL);
	long double complex **FHermitian; malloc2dLongDoubleComplex(&FHermitian, SAMPUTIL, SAMPUTIL);
	long double complex **X4Hermitian; malloc2dLongDoubleComplex(&X4Hermitian, SAMPUTIL, SAMPUTIL);
	long double complex **X4; malloc2dLongDoubleComplex(&X4, SAMPUTIL, SAMPUTIL);
	long double complex **Ryy; malloc2dLongDoubleComplex(&Ryy, SAMPUTIL, SAMPUTIL);
	long double complex **invRyy; malloc2dLongDoubleComplex(&invRyy, SAMPUTIL, SAMPUTIL);
	long double complex **temp1; malloc2dLongDoubleComplex(&temp1, SAMPUTIL, SAMPUTIL);
	long double complex **temp2; malloc2dLongDoubleComplex(&temp2, SAMPUTIL, SAMPUTIL);
	long double complex **temp3; malloc2dLongDoubleComplex(&temp3, SAMPUTIL, SAMPUTIL);
	long double complex **temp4; malloc2dLongDoubleComplex(&temp4, SAMPUTIL, SAMPUTIL);
	long double complex **Id; malloc2dLongDoubleComplex(&Id, SAMPUTIL, SAMPUTIL);

	long double complex **rx_symbols1; malloc2dLongDoubleComplex(&rx_symbols1, SAMPUTIL, 1);
	long double complex **H_EST1; malloc2dLongDoubleComplex(&H_EST1, SAMPUTIL, 1);

	long double **Fmatrix_Re; malloc2dLongDouble(&Fmatrix_Re, SAMPUTIL, SAMPUTIL);
	long double **Fmatrix_Im; malloc2dLongDouble(&Fmatrix_Im, SAMPUTIL, SAMPUTIL);
	long double **Ryy_Re; malloc2dLongDouble(&Ryy_Re, SAMPUTIL, SAMPUTIL);
	long double **Ryy_Im; malloc2dLongDouble(&Ryy_Im, SAMPUTIL, SAMPUTIL);

	if(commonPS->rank==0){

		printf("Processing PS MMSE Interpolation...\n"); 

		for(int r=0; r<SAMPUTIL; r++){
			for(int c=0; c<SAMPUTIL; c++){
				if((r==P0)&&(c==P0))
					X4[r][c] = tx_symbols[P0];
				else if ((r==P1)&&(c==P1))
					X4[r][c] = tx_symbols[P1];
				else if ((r==P2)&&(c==P2))
					X4[r][c] = tx_symbols[P2];
				else if ((r==P3)&&(c==P3))
					X4[r][c] = tx_symbols[P3];
				else
					X4[r][c] = 0.0;

				Fmatrix[c][r] = cexp(-2*I*PI*c*r/SAMPUTIL);
			}
			rx_symbols1[r][0] = rx_symbols[r];					// Vector to Matrix
			H_EST1[r][0] = H_EST_LT[r];							// Vector to Matrix
			H_EST1[r][0] = H_EST_LT[r];							// Vector to Matrix
		}

		complexToDouble(SAMPUTIL,Fmatrix,Fmatrix_Re,Fmatrix_Im);
	}

	// Each Process creates its own FMatrix (necessary for invF)
	MPI_Bcast(&(Fmatrix_Re[0][0]),SAMPUTIL*SAMPUTIL, MPI_LONG_DOUBLE, 0, commonPS->comm);
	MPI_Bcast(&(Fmatrix_Im[0][0]),SAMPUTIL*SAMPUTIL, MPI_LONG_DOUBLE, 0, commonPS->comm);
	doubleToComplex(SAMPUTIL,Fmatrix,Fmatrix_Re,Fmatrix_Im);
	
	MPI_Barrier(MPI_COMM_WORLD);
	Common_PS commonPS_local;
	commonPS_local.rank = commonPS->rank;
	commonPS_local.numprocs = commonPS->numprocs;
	commonPS_local.tag1 = commonPS->tag1;
	commonPS_local.tag2 = commonPS->tag2;
	commonPS_local.tag3 = commonPS->tag3;
	commonPS_local.status = commonPS->status;
	commonPS_local.comm = commonPS->comm;

	MPI_Barrier(MPI_COMM_WORLD);

	inverse_mpi(Fmatrix, SAMPUTIL, invF, &commonPS_local, argc, argv);				//invF
	// inverse_mpi_omp(Fmatrix, SAMPUTIL, invF, &commonPS_local, argc, argv);				//invF

	if(commonPS->rank==0){

		hermitian(Fmatrix,SAMPUTIL,SAMPUTIL,FHermitian);								// FHermitian = F'
		hermitian(X4,SAMPUTIL,SAMPUTIL,X4Hermitian);									// X4Hermitian = X4'
		multiply(invF,SAMPUTIL,SAMPUTIL,H_EST1,SAMPUTIL,1,temp1);						// temp1 = invF*H_EST
		hermitian(temp1,SAMPUTIL,SAMPUTIL,temp2);										// temp2 = (invF*H_EST)'
		multiplyVxVeqM(temp1,SAMPUTIL,SAMPUTIL,temp2,SAMPUTIL,SAMPUTIL,temp3);			// Rhh (temp3) = (invF*H_EST)*(invF*H_EST)'

		multiply(temp3,SAMPUTIL,SAMPUTIL,FHermitian,SAMPUTIL,SAMPUTIL,temp1);			// temp1 = Rhh*F'
		multiply(temp1,SAMPUTIL,SAMPUTIL,X4,SAMPUTIL,SAMPUTIL,temp2);					// Rhy (temp2)

		multiply(temp1,SAMPUTIL,SAMPUTIL,X4Hermitian,SAMPUTIL,SAMPUTIL,temp4);			// temp4 = Rhh*F'*X4'
		multiply(Fmatrix,SAMPUTIL,SAMPUTIL,temp4,SAMPUTIL,SAMPUTIL,temp1);  			// temp1 = F*Rhh*F'*X4'
		multiply(X4,SAMPUTIL,SAMPUTIL,temp1,SAMPUTIL,SAMPUTIL,temp4);					// temp4 = X4*F*Rhh*F'*X4'

		identity(Id,SAMPUTIL,OW2);
		addition(Id,SAMPUTIL,SAMPUTIL,temp4,SAMPUTIL,SAMPUTIL,Ryy);						// Ryy (temp1)

		complexToDouble(SAMPUTIL,Ryy,Ryy_Re,Ryy_Im);	
	}

	MPI_Bcast(&(Ryy_Re[0][0]),SAMPUTIL*SAMPUTIL, MPI_LONG_DOUBLE, 0, commonPS->comm);
	MPI_Bcast(&(Ryy_Im[0][0]),SAMPUTIL*SAMPUTIL, MPI_LONG_DOUBLE, 0, commonPS->comm);
	doubleToComplex(SAMPUTIL,Ryy,Ryy_Re,Ryy_Im);
	
	inverse_mpi(Ryy,SAMPUTIL,invRyy, &commonPS_local, argc, argv);						// invRyy
	// inverse_mpi_omp(Ryy,SAMPUTIL,invRyy, &commonPS_local, argc, argv);						// invRyy

	if(commonPS->rank==0){
	 	multiply(Fmatrix,SAMPUTIL,SAMPUTIL,temp2,SAMPUTIL,SAMPUTIL,temp1);				// temp1 = F*Rhy
		multiply(invRyy,SAMPUTIL,SAMPUTIL,rx_symbols1,SAMPUTIL,1,temp3);				// temp3 = invRyy*rx_symbols
		multiply(temp1,SAMPUTIL,SAMPUTIL,temp3,SAMPUTIL,1,temp2);						// H_EST = F*Rhy*invRyy*rx_symbols
	
		for(int r=0 ; r<SAMPUTIL ; r++)
			H_EST[r] = temp2[r][0];
	}
}

/* ----------------------------------------------------------------------------------------*/
/* -------------------------------- PS MMSE INTERPOLATION (2) -----------------------------*
* 
*  Levels of dependency are analyzed among the functions that are integrated in the MMSE Estimator.
*  Each level contain several functions that are independent. Thus, they can be executed in 
*  parallel. Each function is executed in only 1 processor. The dependency mas as well as a 
*  execution flow graph is shown below. Functions on the same row can be executed in parallel.
*  After their execution, the results are sent to another processor, which will gather the 
*  results from other processors to perform its own task. This system can be seen as a chain of 
*  dependencies, where each level (row) cannot be executed without the completion of the previous
*  
*  levels |       Processor1       |   Processor2   |   Processor3  |   Processor4       |
* -----   |------------------------------------------------------------------------------|
*  01st   | inv(F)      (1)        |    ow2*I (2)   |   F' (3)      |   X4*F (4)         |
*  02nd   | inv(F)*Hest (1)        |                |   F'*X4 (5)   |  (X4*F)' (6)       |
*  03rd   | inv(inv(F)*Hest) (1)   |                |               |                    |
*         | (inv(F)*Hest)*         |                |               |                    |
*  04th   |    inv(inv(F)*Hest)    |                |               |                    |
*         | = Rhh (1)              |                |               |                    |
*  05th   | Rhh*F'*X4 = Rhy	(7)    |                |               |   Rhh*(X4*F)' (8)  |
*  06th   | X4*F*Rhh*(X4*F)' (9)   |   F*Rhy (10)   |               |                    |
*         | ow2*I +                |                |               |                    |
*  07th   |   X4*F*Rhh*(X4*F)'     |                |               |                    |
*         |   =Ryy (11)            |                |               |                    |
*  08th   | inv(Ryy) (11)          |                |               |                    |
*  09th   | F*Rhy*inv(Ryy) (12)    |                |               |                    |
*  10th   | F*Rhy*inv(Ryy)*Rx_symbol (0)            |               |                    |
*/
/* ----------------------------------------------------------------------------------------*/
void WiFi_channel_estimation_PS_MMSE2(long double complex tx_symbols[], long double complex rx_symbols[], long double complex H_EST_LT[], long double complex H_EST[], Common_PS *commonPS, int argc, char *argv[]){

	long double complex **Fmatrix; malloc2dLongDoubleComplex(&Fmatrix, SAMPUTIL, SAMPUTIL);
	long double complex **FHermitian; malloc2dLongDoubleComplex(&FHermitian, SAMPUTIL, SAMPUTIL);
	long double complex **X4Hermitian; malloc2dLongDoubleComplex(&X4Hermitian, SAMPUTIL, SAMPUTIL);
	long double complex **X4; malloc2dLongDoubleComplex(&X4, SAMPUTIL, SAMPUTIL);
	long double complex **invRyy; malloc2dLongDoubleComplex(&invRyy, SAMPUTIL, SAMPUTIL);
	long double complex **invF; malloc2dLongDoubleComplex(&invF, SAMPUTIL, SAMPUTIL);
	long double complex **temp1; malloc2dLongDoubleComplex(&temp1, SAMPUTIL, SAMPUTIL);
	long double complex **temp2; malloc2dLongDoubleComplex(&temp2, SAMPUTIL, SAMPUTIL);
	long double complex **temp3; malloc2dLongDoubleComplex(&temp3, SAMPUTIL, SAMPUTIL);
	long double complex **Id; malloc2dLongDoubleComplex(&Id, SAMPUTIL, SAMPUTIL);

	long double complex **rx_symbols1; malloc2dLongDoubleComplex(&rx_symbols1, SAMPUTIL, 1);
	long double complex **H_EST1; malloc2dLongDoubleComplex(&H_EST1, SAMPUTIL, 1);

	long double **Fmatrix_Re; malloc2dLongDouble(&Fmatrix_Re, SAMPUTIL, SAMPUTIL);
	long double **Fmatrix_Im; malloc2dLongDouble(&Fmatrix_Im, SAMPUTIL, SAMPUTIL);
	long double **FHermitian_Re; malloc2dLongDouble(&FHermitian_Re, SAMPUTIL, SAMPUTIL);
	long double **FHermitian_Im; malloc2dLongDouble(&FHermitian_Im, SAMPUTIL, SAMPUTIL);
	long double **X4Hermitian_Re; malloc2dLongDouble(&X4Hermitian_Re, SAMPUTIL, SAMPUTIL);
	long double **X4Hermitian_Im; malloc2dLongDouble(&X4Hermitian_Im, SAMPUTIL, SAMPUTIL);
	long double **X4_Re; malloc2dLongDouble(&X4_Re, SAMPUTIL, SAMPUTIL);
	long double **X4_Im; malloc2dLongDouble(&X4_Im, SAMPUTIL, SAMPUTIL);
	long double **invF_Re; malloc2dLongDouble(&invF_Re, SAMPUTIL, SAMPUTIL);
	long double **invF_Im; malloc2dLongDouble(&invF_Im, SAMPUTIL, SAMPUTIL);
	long double **Id_Re; malloc2dLongDouble(&Id_Re, SAMPUTIL, SAMPUTIL);
	long double **Id_Im; malloc2dLongDouble(&Id_Im, SAMPUTIL, SAMPUTIL);
	long double **temp1_Re; malloc2dLongDouble(&temp1_Re, SAMPUTIL, SAMPUTIL);
	long double **temp1_Im; malloc2dLongDouble(&temp1_Im, SAMPUTIL, SAMPUTIL);
	long double **temp2_Re; malloc2dLongDouble(&temp2_Re, SAMPUTIL, SAMPUTIL);
	long double **temp2_Im; malloc2dLongDouble(&temp2_Im, SAMPUTIL, SAMPUTIL);
	long double **temp3_Re; malloc2dLongDouble(&temp3_Re, SAMPUTIL, SAMPUTIL);
	long double **temp3_Im; malloc2dLongDouble(&temp3_Im, SAMPUTIL, SAMPUTIL);

	if(commonPS->rank==0){

		printf("Processing PS MMSE Interpolation...\n"); 

		for(int i=0 ; i<SAMPUTIL; i++){
			rx_symbols1[i][0] = rx_symbols[i];
		}

		for(int r=0; r<SAMPUTIL; r++){
			for(int c=0; c<SAMPUTIL; c++){
				if((r==P0)&&(c==P0))
					X4[r][c] = tx_symbols[P0];
				else if ((r==P1)&&(c==P1))
					X4[r][c] = tx_symbols[P1];
				else if ((r==P2)&&(c==P2))
					X4[r][c] = tx_symbols[P2];
				else if ((r==P3)&&(c==P3))
					X4[r][c] = tx_symbols[P3];
				else
					X4[r][c] = 0.0;

				Fmatrix[c][r] = cexp(-2*I*PI*c*r/SAMPUTIL);
			}
		}

		complexToDouble(SAMPUTIL,X4,X4_Re,X4_Im);
		complexToDouble(SAMPUTIL,Fmatrix,Fmatrix_Re,Fmatrix_Im);
	}

	// Process 0 Broadcast Matrices X4 and Fmatrix
	MPI_Bcast(&(X4_Re[0][0]), SAMPUTIL*SAMPUTIL, MPI_LONG_DOUBLE, 0, MPI_COMM_WORLD);
	MPI_Bcast(&(X4_Im[0][0]), SAMPUTIL*SAMPUTIL, MPI_LONG_DOUBLE, 0, MPI_COMM_WORLD);
	MPI_Bcast(&(Fmatrix_Re[0][0]),SAMPUTIL*SAMPUTIL, MPI_LONG_DOUBLE, 0, MPI_COMM_WORLD);
	MPI_Bcast(&(Fmatrix_Im[0][0]),SAMPUTIL*SAMPUTIL, MPI_LONG_DOUBLE, 0, MPI_COMM_WORLD);

	// Each Process creates its own X4 and FMatrix
	doubleToComplex(SAMPUTIL,X4,X4_Re,X4_Im);
	doubleToComplex(SAMPUTIL,Fmatrix,Fmatrix_Re,Fmatrix_Im);

	if(commonPS->rank==1){
		for(int r=0 ; r<SAMPUTIL ; r++)
			H_EST1[r][0] = H_EST_LT[r];
		printf("Proc 1 working on inverse...\n");
		inverse(Fmatrix,SAMPUTIL,invF);
		printf("Proc 1 done with inverse...\n");
		multiply(invF,SAMPUTIL,SAMPUTIL,H_EST1,SAMPUTIL,SAMPUTIL,temp1);		//temp1 = invF*H_EST
		hermitian(temp1,SAMPUTIL,SAMPUTIL,temp2);								//temp2 = (invF*H_EST)'
		multiplyVxVeqM(temp1,SAMPUTIL,SAMPUTIL,temp2,SAMPUTIL,SAMPUTIL,temp3);	//temp3=Rhh
		complexToDouble(SAMPUTIL,temp3,temp3_Re,temp3_Im);
		MPI_Send(&(temp3_Re[0][0]), SAMPUTIL*SAMPUTIL, MPI_LONG_DOUBLE, 7, commonPS->tag1, MPI_COMM_WORLD);
		MPI_Send(&(temp3_Im[0][0]), SAMPUTIL*SAMPUTIL, MPI_LONG_DOUBLE, 7, commonPS->tag2, MPI_COMM_WORLD);
		MPI_Send(&(temp3_Re[0][0]), SAMPUTIL*SAMPUTIL, MPI_LONG_DOUBLE, 8, commonPS->tag1, MPI_COMM_WORLD);
		MPI_Send(&(temp3_Im[0][0]), SAMPUTIL*SAMPUTIL, MPI_LONG_DOUBLE, 8, commonPS->tag2, MPI_COMM_WORLD);
		printf("Proc %d sent its job to 7\n",commonPS->rank);
		printf("Proc %d sent its job to 8\n",commonPS->rank);
	} else if(commonPS->rank==2){
		identity(Id,SAMPUTIL,OW2);
		complexToDouble(SAMPUTIL,Id,Id_Re,Id_Im);		
		MPI_Send(&(Id_Re[0][0]), SAMPUTIL*SAMPUTIL, MPI_LONG_DOUBLE, 11, commonPS->tag1, MPI_COMM_WORLD);
		MPI_Send(&(Id_Im[0][0]), SAMPUTIL*SAMPUTIL, MPI_LONG_DOUBLE, 11, commonPS->tag2, MPI_COMM_WORLD);
		printf("Proc %d sent its job to 11\n",commonPS->rank);
	} else if(commonPS->rank==3){
		hermitian(Fmatrix,SAMPUTIL,SAMPUTIL,FHermitian);
		complexToDouble(SAMPUTIL,FHermitian,FHermitian_Re,FHermitian_Im);  
		MPI_Send(&(FHermitian_Re[0][0]), SAMPUTIL*SAMPUTIL, MPI_LONG_DOUBLE, 5, commonPS->tag1, MPI_COMM_WORLD);
		MPI_Send(&(FHermitian_Im[0][0]), SAMPUTIL*SAMPUTIL, MPI_LONG_DOUBLE, 5, commonPS->tag2, MPI_COMM_WORLD);
		printf("Proc %d sent its job to 5\n",commonPS->rank);
	} else if(commonPS->rank==4){
		multiply(X4,SAMPUTIL,SAMPUTIL,Fmatrix,SAMPUTIL,SAMPUTIL,temp1);
		complexToDouble(SAMPUTIL,temp1,temp1_Re,temp1_Im);
		MPI_Send(&(temp1_Re[0][0]), SAMPUTIL*SAMPUTIL, MPI_LONG_DOUBLE, 6, commonPS->tag1, MPI_COMM_WORLD);
		MPI_Send(&(temp1_Im[0][0]), SAMPUTIL*SAMPUTIL, MPI_LONG_DOUBLE, 6, commonPS->tag2, MPI_COMM_WORLD);
		MPI_Send(&(temp1_Re[0][0]), SAMPUTIL*SAMPUTIL, MPI_LONG_DOUBLE, 9, commonPS->tag1, MPI_COMM_WORLD);
		MPI_Send(&(temp1_Im[0][0]), SAMPUTIL*SAMPUTIL, MPI_LONG_DOUBLE, 9, commonPS->tag2, MPI_COMM_WORLD);
		printf("Proc %d sent its job to 6\n",commonPS->rank);
		printf("Proc %d sent its job to 9\n",commonPS->rank);
	} else if(commonPS->rank==5){
		printf("Proc %d waits...\n",commonPS->rank);
		MPI_Recv(&(temp1_Re[0][0]), SAMPUTIL*SAMPUTIL, MPI_LONG_DOUBLE, 3, commonPS->tag1, MPI_COMM_WORLD, &commonPS->status);
		MPI_Recv(&(temp1_Im[0][0]), SAMPUTIL*SAMPUTIL, MPI_LONG_DOUBLE, 3, commonPS->tag2, MPI_COMM_WORLD, &commonPS->status);
		printf("Proc %d recv job from 3\n",commonPS->rank);
		doubleToComplex(SAMPUTIL,temp1,temp1_Re,temp1_Im);
		multiply(temp1,SAMPUTIL,SAMPUTIL,X4,SAMPUTIL,SAMPUTIL,temp2);			//temp2=F'*X4
		complexToDouble(SAMPUTIL,temp2,temp2_Re,temp2_Im);
		MPI_Send(&(temp2_Re[0][0]), SAMPUTIL*SAMPUTIL, MPI_LONG_DOUBLE, 7, commonPS->tag1, MPI_COMM_WORLD);
		MPI_Send(&(temp2_Im[0][0]), SAMPUTIL*SAMPUTIL, MPI_LONG_DOUBLE, 7, commonPS->tag2, MPI_COMM_WORLD);
		printf("Proc %d sent its job to 7\n",commonPS->rank);
	} else if(commonPS->rank==6){
		printf("Proc %d waits...\n",commonPS->rank);
		MPI_Recv(&(temp1_Re[0][0]), SAMPUTIL*SAMPUTIL, MPI_LONG_DOUBLE, 4, commonPS->tag1, MPI_COMM_WORLD, &commonPS->status);
		MPI_Recv(&(temp1_Im[0][0]), SAMPUTIL*SAMPUTIL, MPI_LONG_DOUBLE, 4, commonPS->tag2, MPI_COMM_WORLD, &commonPS->status);
		printf("Proc %d recv job from 4\n",commonPS->rank);
		doubleToComplex(SAMPUTIL,temp1,temp1_Re,temp1_Re);						//temp1=X4*F
		hermitian(temp1,SAMPUTIL,SAMPUTIL,temp2);								//temp2=(X4*F)'
		complexToDouble(SAMPUTIL,temp2,temp2_Re,temp2_Im);
		MPI_Send(&(temp2_Re[0][0]), SAMPUTIL*SAMPUTIL, MPI_LONG_DOUBLE, 8, commonPS->tag1, MPI_COMM_WORLD);
		MPI_Send(&(temp2_Im[0][0]), SAMPUTIL*SAMPUTIL, MPI_LONG_DOUBLE, 8, commonPS->tag2, MPI_COMM_WORLD);
		MPI_Send(&(temp2_Re[0][0]), SAMPUTIL*SAMPUTIL, MPI_LONG_DOUBLE, 9, commonPS->tag1, MPI_COMM_WORLD);
		MPI_Send(&(temp2_Im[0][0]), SAMPUTIL*SAMPUTIL, MPI_LONG_DOUBLE, 9, commonPS->tag2, MPI_COMM_WORLD);
		printf("Proc %d sent its job to 8\n",commonPS->rank);
		printf("Proc %d sent its job to 9\n",commonPS->rank);
	} else if(commonPS->rank==7){
		printf("Proc %d waits...\n",commonPS->rank);
		MPI_Recv(&(temp1_Re[0][0]), SAMPUTIL*SAMPUTIL, MPI_LONG_DOUBLE, 5, commonPS->tag1, MPI_COMM_WORLD, &commonPS->status);
		MPI_Recv(&(temp1_Im[0][0]), SAMPUTIL*SAMPUTIL, MPI_LONG_DOUBLE, 5, commonPS->tag2, MPI_COMM_WORLD, &commonPS->status);
		printf("Proc %d recv job from 5\n",commonPS->rank);
		MPI_Recv(&(temp2_Re[0][0]), SAMPUTIL*SAMPUTIL, MPI_LONG_DOUBLE, 1, commonPS->tag1, MPI_COMM_WORLD, &commonPS->status);
		MPI_Recv(&(temp2_Im[0][0]), SAMPUTIL*SAMPUTIL, MPI_LONG_DOUBLE, 1, commonPS->tag2, MPI_COMM_WORLD, &commonPS->status);
		printf("Proc %d recv job from 1\n",commonPS->rank);
		doubleToComplex(SAMPUTIL,temp1,temp1_Re,temp1_Re);						//temp1=F'*X4
		doubleToComplex(SAMPUTIL,temp2,temp2_Re,temp2_Re);						//temp2=Rhh
		multiply(temp2,SAMPUTIL,SAMPUTIL,temp1,SAMPUTIL,SAMPUTIL,temp3);		//temp3=Rhy=Rhh*F'*X4
		complexToDouble(SAMPUTIL,temp3,temp3_Re,temp3_Im);
		MPI_Send(&(temp3_Re[0][0]), SAMPUTIL*SAMPUTIL, MPI_LONG_DOUBLE, 10, commonPS->tag1, MPI_COMM_WORLD);
		MPI_Send(&(temp3_Im[0][0]), SAMPUTIL*SAMPUTIL, MPI_LONG_DOUBLE, 10, commonPS->tag2, MPI_COMM_WORLD);
		printf("Proc %d sent its job to 10\n",commonPS->rank);
	} else if(commonPS->rank==8){
		printf("Proc %d waits...\n",commonPS->rank);
		MPI_Recv(&(temp1_Re[0][0]), SAMPUTIL*SAMPUTIL, MPI_LONG_DOUBLE, 6, commonPS->tag1, MPI_COMM_WORLD, &commonPS->status);
		MPI_Recv(&(temp1_Im[0][0]), SAMPUTIL*SAMPUTIL, MPI_LONG_DOUBLE, 6, commonPS->tag2, MPI_COMM_WORLD, &commonPS->status);
		printf("Proc %d recv job from 6\n",commonPS->rank);
		printf("Proc %d waits...\n",commonPS->rank);
		MPI_Recv(&(temp2_Re[0][0]), SAMPUTIL*SAMPUTIL, MPI_LONG_DOUBLE, 1, commonPS->tag1, MPI_COMM_WORLD, &commonPS->status);
		MPI_Recv(&(temp2_Im[0][0]), SAMPUTIL*SAMPUTIL, MPI_LONG_DOUBLE, 1, commonPS->tag2, MPI_COMM_WORLD, &commonPS->status);
		printf("Proc %d recv job from 1\n",commonPS->rank);
		doubleToComplex(SAMPUTIL,temp1,temp1_Re,temp1_Re);						//temp1=(X4*F)'
		doubleToComplex(SAMPUTIL,temp2,temp2_Re,temp2_Re);						//temp2=Rhh
		multiply(temp2,SAMPUTIL,SAMPUTIL,temp1,SAMPUTIL,SAMPUTIL,temp3);		//temp3=Rhh*(X4*F)'
		complexToDouble(SAMPUTIL,temp3,temp3_Re,temp3_Im);
		MPI_Send(&(temp3_Re[0][0]), SAMPUTIL*SAMPUTIL, MPI_LONG_DOUBLE, 9, commonPS->tag1, MPI_COMM_WORLD);
		MPI_Send(&(temp3_Im[0][0]), SAMPUTIL*SAMPUTIL, MPI_LONG_DOUBLE, 9, commonPS->tag2, MPI_COMM_WORLD);
		printf("Proc %d sent its job to 9\n",commonPS->rank);
	} else if(commonPS->rank==9){
		printf("Proc %d waits...\n",commonPS->rank);
		MPI_Recv(&(temp1_Re[0][0]), SAMPUTIL*SAMPUTIL, MPI_LONG_DOUBLE, 4, commonPS->tag1, MPI_COMM_WORLD, &commonPS->status);
		MPI_Recv(&(temp1_Im[0][0]), SAMPUTIL*SAMPUTIL, MPI_LONG_DOUBLE, 4, commonPS->tag2, MPI_COMM_WORLD, &commonPS->status);
		printf("Proc %d recv job from 4\n",commonPS->rank);
		printf("Proc %d waits...\n",commonPS->rank);
		MPI_Recv(&(temp2_Re[0][0]), SAMPUTIL*SAMPUTIL, MPI_LONG_DOUBLE, 8, commonPS->tag1, MPI_COMM_WORLD, &commonPS->status);
		MPI_Recv(&(temp2_Im[0][0]), SAMPUTIL*SAMPUTIL, MPI_LONG_DOUBLE, 8, commonPS->tag2, MPI_COMM_WORLD, &commonPS->status);
		printf("Proc %d recv job from 8\n",commonPS->rank);
		doubleToComplex(SAMPUTIL,temp1,temp1_Re,temp1_Re);						//temp1=X4*F
		doubleToComplex(SAMPUTIL,temp2,temp2_Re,temp2_Re);						//temp2=Rhh*(X4*F)'
		multiply(temp1,SAMPUTIL,SAMPUTIL,temp2,SAMPUTIL,SAMPUTIL,temp3);		//temp3=X4*F*Rhh*(X4*F)'
		complexToDouble(SAMPUTIL,temp3,temp3_Re,temp3_Im);
		MPI_Send(&(temp3_Re[0][0]), SAMPUTIL*SAMPUTIL, MPI_LONG_DOUBLE, 11, commonPS->tag1, MPI_COMM_WORLD);
		MPI_Send(&(temp3_Im[0][0]), SAMPUTIL*SAMPUTIL, MPI_LONG_DOUBLE, 11, commonPS->tag2, MPI_COMM_WORLD);
		printf("Proc %d sent its job to 11\n",commonPS->rank);
	} else if(commonPS->rank==10){
		printf("Proc %d waits...\n",commonPS->rank);
		MPI_Recv(&(temp1_Re[0][0]), SAMPUTIL*SAMPUTIL, MPI_LONG_DOUBLE, 7, commonPS->tag1, MPI_COMM_WORLD, &commonPS->status);
		MPI_Recv(&(temp1_Im[0][0]), SAMPUTIL*SAMPUTIL, MPI_LONG_DOUBLE, 7, commonPS->tag2, MPI_COMM_WORLD, &commonPS->status);
		printf("Proc %d recv job from 7\n",commonPS->rank);
		doubleToComplex(SAMPUTIL,temp1,temp1_Re,temp1_Re);						//temp1=Rhy
		multiply(Fmatrix,SAMPUTIL,SAMPUTIL,temp1,SAMPUTIL,SAMPUTIL,temp2);		//temp2=F*Rhy
		complexToDouble(SAMPUTIL,temp2,temp2_Re,temp2_Im);
		MPI_Send(&(temp2_Re[0][0]), SAMPUTIL*SAMPUTIL, MPI_LONG_DOUBLE, 12, commonPS->tag1, MPI_COMM_WORLD);
		MPI_Send(&(temp2_Im[0][0]), SAMPUTIL*SAMPUTIL, MPI_LONG_DOUBLE, 12, commonPS->tag2, MPI_COMM_WORLD);
		printf("Proc %d sent its job to 12\n",commonPS->rank);
	} else if(commonPS->rank==11){
		printf("Proc %d waits...\n",commonPS->rank);
		MPI_Recv(&(temp1_Re[0][0]), SAMPUTIL*SAMPUTIL, MPI_LONG_DOUBLE, 2, commonPS->tag1, MPI_COMM_WORLD, &commonPS->status);
		MPI_Recv(&(temp1_Im[0][0]), SAMPUTIL*SAMPUTIL, MPI_LONG_DOUBLE, 2, commonPS->tag2, MPI_COMM_WORLD, &commonPS->status);
		printf("Proc %d recv job from 2\n",commonPS->rank);
		printf("Proc %d waits...\n",commonPS->rank);
		MPI_Recv(&(temp2_Re[0][0]), SAMPUTIL*SAMPUTIL, MPI_LONG_DOUBLE, 9, commonPS->tag1, MPI_COMM_WORLD, &commonPS->status);
		MPI_Recv(&(temp2_Im[0][0]), SAMPUTIL*SAMPUTIL, MPI_LONG_DOUBLE, 9, commonPS->tag2, MPI_COMM_WORLD, &commonPS->status);
		printf("Proc %d recv job from 9\n",commonPS->rank);
		doubleToComplex(SAMPUTIL,temp1,temp1_Re,temp1_Re);						//temp1=ow2*I
		doubleToComplex(SAMPUTIL,temp2,temp2_Re,temp2_Re);						//temp2=X4*F*Rhh*(X4*F)'
		addition(temp1,SAMPUTIL,SAMPUTIL,temp2,SAMPUTIL,SAMPUTIL,temp3);		//temp3=ow2*I+X4*F*Rhh*(X4*F)'=Ryy
		printf("Proc 11 working on inverse...\n");
		inverse(temp3,SAMPUTIL,temp1);											//temp1=inv(Ryy)
		printf("Proc 11 done with inverse...\n");
		complexToDouble(SAMPUTIL,temp1,temp1_Re,temp1_Im);
		MPI_Send(&(temp1_Re[0][0]), SAMPUTIL*SAMPUTIL, MPI_LONG_DOUBLE, 12, commonPS->tag1, MPI_COMM_WORLD);
		MPI_Send(&(temp1_Im[0][0]), SAMPUTIL*SAMPUTIL, MPI_LONG_DOUBLE, 12, commonPS->tag2, MPI_COMM_WORLD);
		printf("Proc %d sent its job to 12\n",commonPS->rank);
	} else if(commonPS->rank==12){
		printf("Proc %d waits...\n",commonPS->rank);
		MPI_Recv(&(temp1_Re[0][0]), SAMPUTIL*SAMPUTIL, MPI_LONG_DOUBLE, 10, commonPS->tag1, MPI_COMM_WORLD, &commonPS->status);
		MPI_Recv(&(temp1_Im[0][0]), SAMPUTIL*SAMPUTIL, MPI_LONG_DOUBLE, 10, commonPS->tag2, MPI_COMM_WORLD, &commonPS->status);
		printf("Proc %d recv job from 10\n",commonPS->rank);
		printf("Proc %d waits...\n",commonPS->rank);
		MPI_Recv(&(temp2_Re[0][0]), SAMPUTIL*SAMPUTIL, MPI_LONG_DOUBLE, 11, commonPS->tag1, MPI_COMM_WORLD, &commonPS->status);
		MPI_Recv(&(temp2_Im[0][0]), SAMPUTIL*SAMPUTIL, MPI_LONG_DOUBLE, 11, commonPS->tag2, MPI_COMM_WORLD, &commonPS->status);
		printf("Proc %d recv job from 11\n",commonPS->rank);
		doubleToComplex(SAMPUTIL,temp1,temp1_Re,temp1_Re);						//temp1=F*Rhy
		doubleToComplex(SAMPUTIL,temp2,temp2_Re,temp2_Re);						//temp2=inv(Ryy)
		multiply(temp1,SAMPUTIL,SAMPUTIL,temp2,SAMPUTIL,SAMPUTIL,temp3);		//temp3=F*Rhy*inv(Ryy)
		complexToDouble(SAMPUTIL,temp3,temp3_Re,temp3_Im);
		MPI_Send(&(temp3_Re[0][0]), SAMPUTIL*SAMPUTIL, MPI_LONG_DOUBLE, 0, commonPS->tag1, MPI_COMM_WORLD);
		MPI_Send(&(temp3_Im[0][0]), SAMPUTIL*SAMPUTIL, MPI_LONG_DOUBLE, 0, commonPS->tag2, MPI_COMM_WORLD);
		printf("Proc %d sent its job to 0\n",commonPS->rank);
	} else if(commonPS->rank == 0){
		printf("Proc %d waits...\n",commonPS->rank);
		MPI_Recv(&(temp1_Re[0][0]), SAMPUTIL*SAMPUTIL, MPI_LONG_DOUBLE, 12, commonPS->tag1, MPI_COMM_WORLD, &commonPS->status);
		MPI_Recv(&(temp1_Im[0][0]), SAMPUTIL*SAMPUTIL, MPI_LONG_DOUBLE, 12, commonPS->tag2, MPI_COMM_WORLD, &commonPS->status);
		printf("Proc %d recv job from 12\n",commonPS->rank);
		doubleToComplex(SAMPUTIL,temp1,temp1_Re,temp1_Re);						//temp1=F*Rhy*inv(Ryy)
		multiply(temp1,SAMPUTIL,SAMPUTIL,rx_symbols1,SAMPUTIL,SAMPUTIL,temp2);	//temp3=F*Rhy*inv(Ryy)*rx_symbols1

		for(int r=0 ; r<SAMPUTIL ; r++)
			H_EST[r] = temp2[r][0];
	}
}

/*OUTPUT 1
MPI
20 -> 22760000.000000
15 -> 24870000.000000
10 -> 33350000.000000
05 -> 50800000.000000
02 -> 110600000.000000
01 -> 208640000.000000

MPI + OpenMP
20 -> 10690000.000000
15 -> 11650000.000000
10 -> 13410000.000000
05 -> 19480000.000000
02 -> 42810000.000000
01 -> 81380000.000000

OUTPUT 2 - MPI + OpenMP
PS MMSE Interpolation (1) - groups: 1 Proc. per group: 20
PS MMSE Interpolation (1) - Elapsed time 10470000.000000
PS MMSE Interpolation (1) - Equival time 10470000.000000

PS MMSE Interpolation (1) - groups: 2 Proc. per group: 10
PS MMSE Interpolation (1) - Elapsed time 17430000.000000/2=8720000
PS MMSE Interpolation (1) - Equival time 10470000

PS MMSE Interpolation (1) - groups: 4 Proc. per group: 5
PS MMSE Interpolation (1) - Elapsed time 26060000.000000/4=6520000
PS MMSE Interpolation (1) - Equival time 10470000

PS MMSE Interpolation (1) - groups: 5 Proc. per group: 4
PS MMSE Interpolation (1) - Elapsed time 33950000.000000/5=6790000
PS MMSE Interpolation (1) - Equival time 10470000

PS MMSE Interpolation (1) - groups: 10 Proc. per group: 2
PS MMSE Interpolation (1) - Elapsed time 57360000.000000=5740000
PS MMSE Interpolation (1) - Equival time 10470000

PS MMSE Interpolation (1) - groups: 20 Proc. per group: 1
PS MMSE Interpolation (1) - Elapsed time 109650000.000000/20=5490000
PS MMSE Interpolation (1) - Equival time 10470000

OUTPUT 3 - MPI
PS MMSE Interpolation (1) - groups: 1 Proc. per group: 20
PS MMSE Interpolation (1) - Elapsed time 24160000.000000
PS MMSE Interpolation (1) - Equival time 24160000.000000

PS MMSE Interpolation (1) - groups: 2 Proc. per group: 10
PS MMSE Interpolation (1) - Elapsed time 38140000.000000/2=19070000.000000
PS MMSE Interpolation (1) - Equival time 24160000.000000

PS MMSE Interpolation (1) - groups: 4 Proc. per group: 5
PS MMSE Interpolation (1) - Elapsed time 64840000.000000/4=16210000.000000
PS MMSE Interpolation (1) - Equival time 24160000.000000

PS MMSE Interpolation (1) - groups: 5 Proc. per group: 4
PS MMSE Interpolation (1) - Elapsed time 80390000.000000/5=16070000.000000
PS MMSE Interpolation (1) - Equival time 24160000.000000

PS MMSE Interpolation (1) - groups: 10 Proc. per group: 2
PS MMSE Interpolation (1) - Elapsed time 146370000.000000/10=14637000.000000
PS MMSE Interpolation (1) - Equival time 24160000.000000

PS MMSE Interpolation (1) - groups: 20 Proc. per group: 1
PS MMSE Interpolation (1) - Elapsed time 274340000.000000/20=13710000.000000
PS MMSE Interpolation (1) - Equival time 24160000.000000
*/