#define MAZESIZE_x 10
#define MAZESIZE_y 10  

t_bool is_unknown(int x, int y);

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
    t_bool change_flag;

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
                    if((wall[i][j].north & mask) == NOMAL)
                    {
                        if(map[i][j+1] == 255){
                            map[i][j+1] = map[i][j] + 1;
                            change_flag = true;
                        }
                    }
                }

                if(i < MAZESIZE_x-1){
                    if((wall[i][j].east & mask) == NOMAL)
                    {
                        if(map[i+1][j] == 255){
                            map[i+1][j] = map[i][j] + 1;
                            change_flag = true;
                        }
                    }
                }

                if(j > 0){
                    if((wall[i][j].south & mask) == NOMAL)
                    {
                        if(map[i][j-1] == 255){
                            map[i][j-1] = map[i][j] + 1;
                            change_flag = true;
                        }
                    }
                }
                if(i > 0){
                    if((wall[i][j].west & mask) == NOMAL)
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
            s_write = NOMALL;

            break;
        
        case east:
            
            n_write = CONV_SEN2WALl(sen_fr.is_wall);
            e_write = CONV_SEN2WALl(sen_r.is_wall || sen_br.is_wall);
            w_write = CONV_SEN2WALl(sen_fl.is_wall);
            s_write = NOMALL;

            break;
        
        case south:
            
            n_write = CONV_SEN2WALl(sen_fl.is_wall);
            e_write = CONV_SEN2WALl(sen_r.is_wall);
            w_write = CONV_SEN2WALl(sen_fr.is_wall || sen_br.is_wall);
            s_write = NOMALL;

            break;

        case west:
            
            n_write = CONV_SEN2WALl(sen_l.is_wall);
            e_write = CONV_SEN2WALl(sen_fr.is_wall);
            w_write = CONV_SEN2WALl(sen_r.is_wall || sen_br.is_wall);
            s_write = NOMALL;

            break;

        default:
            break;
    }
}

void mause(){
    
}