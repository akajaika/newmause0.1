
#ifdef _MYTYPEDEF

#else

typedef struct
{
	short value;	
	short d_value;		
	short p_value;		
	short p2_value;
	short error;	
	short ref;		
	short th_wall;	
	short th_control;	
	bool is_wall;		
	bool is_control;	
}t_sensor;			

typedef struct
{
	float control;	
	float omega;	
	float p_omega;
	float theta;	
	float p_theta;		
	float error;		
	float p_error;		
	float diff;	
	float sum;	
	float sum_max;		
	float kp;		
	float kd;	
	float ki;		
	bool enable;		
}t_control;		

typedef struct
{
	bool enable;
}t_testmode;

typedef struct
{	
	short duty;
}t_duty;

typedef struct
{
	t_duty motor_r;
	t_duty motor_l;
}t_motor;

extern t_motor motor;	

typedef enum
{
	front=0,	
	right=1,		
	rear=2,			
	left=3,			
	unknown,	
}t_local_dir;	

typedef enum
{
	north=0,
	east=1,
	south=2,
	west=3,
}t_direction;

typedef struct
{
	short x;
	short y;
	t_direction dir;
}t_position;

typedef struct
{
	short x;
	short y;
	t_direction dir;
}next_position;

typedef struct
{
	unsigned char north:2;	
	unsigned char east:2;	
	unsigned char south:2;	
	unsigned char west:2;	
}t_wall;			


#define _MYTYPEDEF

#endif
