#include "static_parameters.hpp"
#include "typedef.hpp"
#include "parameters.hpp"
#include "glob_ver.hpp"

#define MAZESIZE_x 16
#define MAZESIZE_y 16

inline int CONV_SEN2WALl(bool w) {
    return w ? WALL : NOWALL;
}

int map[MAZESIZE_x][MAZESIZE_y];

bool is_unknown(int x, int y);

void init_map(int x, int y){
    int i,j;

    for(i=0;i<MAZESIZE_x;i++){
        for(j=0;j<MAZESIZE_y;j++){
            map[i][j] = 255;
        }
    }

    map[x][y] = 0;
}

void make_map(int x, int y){
    int i,j;
    bool change_flag;

    init_map(x,y);
    
    do
    {
        change_flag = false;
        for(i=0;i<MAZESIZE_x;i++){
            for(j=0;j<MAZESIZE_y;j++){
                if(map[i][j] == 255){
                    continue;
                }

                if(j < MAZESIZE_y-1){
                    if((wall[i][j].north & mask) == NOWALL)
                    {
                        if(map[i][j+1] == 255){
                            map[i][j+1] = map[i][j] + 1;
                            change_flag = true;
                        }
                    }
                }

                if(i < MAZESIZE_x-1){
                    if((wall[i][j].east & mask) == NOWALL)
                    {
                        if(map[i+1][j] == 255){
                            map[i+1][j] = map[i][j] + 1;
                            change_flag = true;
                        }
                    }
                }

                if(j > 0){
                    if((wall[i][j].south & mask) == NOWALL)
                    {
                        if(map[i][j-1] == 255){
                            map[i][j-1] = map[i][j] + 1;
                            change_flag = true;
                        }
                    }
                }
                if(i > 0){
                    if((wall[i][j].west & mask) == NOWALL)
                    {
                        if(map[i-1][j] == 255){
                            map[i-1][j] = map[i][j] + 1;
                            change_flag = true;
                        }
                    }
                }
            }
        }// 255のマスを0にする
    } while (change_flag == true);
}

void set_wall(int x, int y){
    int n_write,s_write,e_write,w_write;

    switch (mypos.dir)
    {
        case north:
            
            n_write = CONV_SEN2WALl(sen_fr.is_wall || sen_fl.is_wall);
            e_write = CONV_SEN2WALl(sen_r.is_wall);
            w_write = CONV_SEN2WALl(sen_l.is_wall);
            s_write = NOWALL;

            break;
        
        case east:
            
            n_write = CONV_SEN2WALl(sen_l.is_wall);
            e_write = CONV_SEN2WALl(sen_fr.is_wall || sen_fl.is_wall);
            w_write = NOWALL;
            s_write = CONV_SEN2WALl(sen_r.is_wall);

            break;
        
        case south:
            
            n_write = NOWALL;
            e_write = CONV_SEN2WALl(sen_l.is_wall);
            w_write = CONV_SEN2WALl(sen_r.is_wall);
            s_write = CONV_SEN2WALl(sen_fr.is_wall || sen_fl.is_wall);

            break;

        case west:
            
            n_write = CONV_SEN2WALl(sen_r.is_wall);
            e_write = NOWALL;
            w_write = CONV_SEN2WALl(sen_fr.is_wall || sen_fl.is_wall);
            s_write = CONV_SEN2WALl(sen_l.is_wall);

            break;

        default:
            break;
    }

    wall[x][y].north = n_write;	//北側の壁情報を更新
    wall[x][y].east = e_write;	//東側の壁情報を更新
    wall[x][y].south = s_write;	//南側の壁情報を更新
    wall[x][y].west = w_write;	//西側の壁情報を更新

    if(y < MAZESIZE_Y-1)
	{
		wall[x][y+1].south = n_write;	
	}
    if(x < MAZESIZE_X-1)
    {
        wall[x+1][y].west = e_write;	
    }
    if(y > 0)
    {
        wall[x][y-1].north = s_write;	
    }
    if(x > 0)
    {
        wall[x-1][y].east = w_write;	
    }
}

bool is_unknown(int x, int y)	//指定された区画が未探索か否かを判断する関数 未探索:true　探索済:false
{
	//座標x,yが未探索区間か否かを調べる
	
	if((wall[x][y].north == UNKNOWN) || (wall[x][y].east == UNKNOWN) || (wall[x][y].south == UNKNOWN) || (wall[x][y].west == UNKNOWN))
	{			//どこかの壁情報が不明のままであれば
		return true;	//未探索
	}
	else
	{
		return false;	//探索済
	}
}

int get_priority(int x, int y, t_direction dir)	//優先度を取得する関数
{
    int priority = 0;
    
    if (mypos.dir == dir)
    {
        priority = 2;	
    }
    else if ((4+mypos.dir-dir)/4 == 2)
    {
        priority = 0;	//優先度1
    }
    else {
        priority = 1;	//優先度3
    }

    if(is_unknown(x,y) == true)
    {
        priority += 4;	//優先度2
    }

    return priority;	//優先度を返す
}





void mause(){
    
}