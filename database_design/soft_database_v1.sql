/*==============================================================*/
/* DBMS name:      MySQL 5.0                                    */
/* Created on:     2020/5/31 15:48:14                           */
/*==============================================================*/


/*==============================================================*/
/* Table: clipboard                                             */
/*==============================================================*/
create table clipboard
(
   id                   int not null,
   file_hash            char(128) not null,
   time                 datetime not null,
   primary key (id, file_hash)
);

/*==============================================================*/
/* Table: files_save                                            */
/*==============================================================*/
create table files_save
(
   file_hash            char(128) not null,
   file_name            varchar(100) not null,
   is_folder            bool not null,
   file_size            int,
   file_path            varchar(100),
   primary key (file_hash)
);

/*==============================================================*/
/* Table: id_pwd                                                */
/*==============================================================*/
create table id_pwd
(
   id                   int not null auto_increment,
   pwd_hash             char(128) not null,
   locked               bool not null,
   wrong_num            int not null,
   primary key (id)
);

/*==============================================================*/
/* Table: user                                                  */
/*==============================================================*/
create table user
(
   user_name            varchar(30) not null,
   id                   int not null,
   mode_code            tinyint not null,
   img_url              varchar(100),
   primary key (user_name)
);

/*==============================================================*/
/* Table: user_mode_right                                       */
/*==============================================================*/
create table user_mode_right
(
   mode_code            tinyint not null,
   max_space            int not null,
   primary key (mode_code)
);

/*==============================================================*/
/* Table: who_has_what                                          */
/*==============================================================*/
create table who_has_what
(
   id                   int not null,
   file_hash            char(128) not null,
   vir_upper_dic        varchar(100) not null,
   primary key (id, file_hash)
);

alter table clipboard add constraint FK_clipboard foreign key (id)
      references id_pwd (id) on delete restrict on update restrict;

alter table clipboard add constraint FK_clipboard2 foreign key (file_hash)
      references files_save (file_hash) on delete restrict on update restrict;

alter table user add constraint FK_login_info foreign key (id)
      references id_pwd (id) on delete restrict on update restrict;

alter table user add constraint FK_user_right_link foreign key (mode_code)
      references user_mode_right (mode_code) on delete restrict on update restrict;

alter table who_has_what add constraint FK_who_has_what foreign key (id)
      references id_pwd (id) on delete restrict on update restrict;

alter table who_has_what add constraint FK_who_has_what2 foreign key (file_hash)
      references files_save (file_hash) on delete restrict on update restrict;

