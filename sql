 CREATE TABLE user (
  id int NOT NULL,
  password varchar(20) NOT NULL,
  name varchar(20) NOT NULL,
  image varchar(200) NOT NULL,
  PRIMARY KEY (id)
);
 CREATE TABLE friendship (
  id1 int NOT NULL,
  id2 int NOT NULL,
  isFriend tinyint NOT NULL,
  PRIMARY KEY (id1,id2),
  FOREIGN KEY (id1) REFERENCES user(id),
  FOREIGN KEY (id2) REFERENCES user(id)
);
