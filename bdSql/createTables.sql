
-- Table: public.appnames
DROP TABLE IF EXISTS public.appnames CASCADE;
CREATE TABLE public.appnames
(
  app     SERIAL,
  appname character varying NOT NULL,
  CONSTRAINT appnames_pkey PRIMARY KEY (app)
) WITH (OIDS=FALSE);
ALTER TABLE public.appnames OWNER TO postgres;



-- Table: public.sourcer
DROP TABLE IF EXISTS public.sourcer CASCADE;
CREATE TABLE public.sourcer
(
  sourcer SERIAL,
  name    character varying NOT NULL,
  CONSTRAINT sourcer_pkey PRIMARY KEY (sourcer)
)
WITH (OIDS=FALSE);
ALTER TABLE public.sourcer OWNER TO postgres;



-- Table: public.msgtypes
DROP TABLE IF EXISTS public.msgtypes CASCADE;
CREATE TABLE public.msgtypes
(
  msgtype  SERIAL,
  typename character varying,
  CONSTRAINT msgtypes_pkey PRIMARY KEY (msgtype)
)
WITH (OIDS=FALSE);
ALTER TABLE public.msgtypes OWNER TO postgres;



-- Table: public.messages
DROP TABLE IF EXISTS public.messages CASCADE;
CREATE TABLE public.messages
(
  id        bigserial,
  idst      integer,
  idkrug    integer DEFAULT 0,
  host      inet,
  sourcer   integer,
  app       integer,
  msgtype   integer,
  dttm      timestamp with time zone DEFAULT now(),
  dttmsaved timestamp with time zone DEFAULT now(),
  message   character varying NOT NULL,

  CONSTRAINT messages_pkey PRIMARY KEY (id),
  CONSTRAINT app_outkey FOREIGN KEY (app)
      REFERENCES public.appnames (app) MATCH SIMPLE
      ON UPDATE NO ACTION ON DELETE NO ACTION,
  CONSTRAINT sourcer_okey FOREIGN KEY (sourcer)
      REFERENCES public.sourcer (sourcer) MATCH SIMPLE
      ON UPDATE NO ACTION ON DELETE NO ACTION,
  CONSTRAINT type_okey FOREIGN KEY (msgtype)
      REFERENCES public.msgtypes (msgtype) MATCH SIMPLE
      ON UPDATE NO ACTION ON DELETE NO ACTION
)
WITH (OIDS=FALSE);
ALTER TABLE public.messages OWNER TO postgres;

-- Index: public.fki_app_outkey
DROP INDEX IF EXISTS public.fki_app_outkey;
CREATE INDEX fki_app_outkey
  ON public.messages
  USING btree
  (app);

-- Index: public.fki_sourcer_okey
DROP INDEX IF EXISTS public.fki_sourcer_okey;
CREATE INDEX fki_sourcer_okey
  ON public.messages
  USING btree
  (sourcer);

-- Index: public.fki_type_okey
DROP INDEX IF EXISTS public.fki_type_okey;
CREATE INDEX fki_type_okey
  ON public.messages
  USING btree
  (msgtype);

-- Index: public.messages_message_idx
DROP INDEX IF EXISTS public.messages_message_idx;
CREATE INDEX messages_message_idx
  ON public.messages
  USING btree
  (message COLLATE pg_catalog."default");

INSERT INTO appnames (app,appname)VALUES(1,'АРМ-И');
INSERT INTO appnames (app,appname)VALUES(2,'АРМ-С');
INSERT INTO appnames (app,appname)VALUES(3,'АРМ-К');
INSERT INTO appnames (app,appname)VALUES(4,'АРМ-У');
INSERT INTO appnames (app,appname)VALUES(5,'АРМ ШН');
INSERT INTO appnames (app,appname)VALUES(6,'BridgeSetun');
INSERT INTO appnames (app,appname)VALUES(7,'АРМ ШНЦ');
INSERT INTO appnames (app,appname)VALUES(8,'Управление');
INSERT INTO appnames (app,appname)VALUES(9,'Станция связи');
INSERT INTO appnames (app,appname)VALUES(10,'График');
INSERT INTO appnames (app,appname)VALUES(11,' ');
INSERT INTO appnames (app,appname)VALUES(12,' ');
INSERT INTO appnames (app,appname)VALUES(13,' ');

INSERT INTO sourcer (sourcer,name)VALUES(1,'Связь');
INSERT INTO sourcer (sourcer,name)VALUES(2,'NetAgent');
INSERT INTO sourcer (sourcer,name)VALUES(3,'АРМ-32');
INSERT INTO sourcer (sourcer,name)VALUES(4,'Управление');
INSERT INTO sourcer (sourcer,name)VALUES(5,'Контроль');
INSERT INTO sourcer (sourcer,name)VALUES(6,'Cвязь с АСОУП');
INSERT INTO sourcer (sourcer,name)VALUES(7,'Диагностика');
INSERT INTO sourcer (sourcer,name)VALUES(8,'Сеть');
INSERT INTO sourcer (sourcer,name)VALUES(9,'СПОК');
INSERT INTO sourcer (sourcer,name)VALUES(10,'СУБД');
INSERT INTO sourcer (sourcer,name)VALUES(11,'Шлюз Сетунь');
INSERT INTO sourcer (sourcer,name)VALUES(12,'Шлюз СПД');
INSERT INTO sourcer (sourcer,name)VALUES(13,'Логический контроль');
INSERT INTO sourcer (sourcer,name)VALUES(14,'-');
INSERT INTO sourcer (sourcer,name)VALUES(15,'-');
INSERT INTO sourcer (sourcer,name)VALUES(16,'-');
INSERT INTO sourcer (sourcer,name)VALUES(17,'-');

INSERT INTO msgtypes (msgtype,typename)VALUES(1,'Аварийное');
INSERT INTO msgtypes (msgtype,typename)VALUES(2,'Технологическое');
INSERT INTO msgtypes (msgtype,typename)VALUES(3,'Системное');
INSERT INTO msgtypes (msgtype,typename)VALUES(4,'Диагностическое');
INSERT INTO msgtypes (msgtype,typename)VALUES(5,'Программное');
INSERT INTO msgtypes (msgtype,typename)VALUES(6,'Фатальная ошибка');
INSERT INTO msgtypes (msgtype,typename)VALUES(7,'Уведомление');
INSERT INTO msgtypes (msgtype,typename)VALUES(8,'ТУ');
INSERT INTO msgtypes (msgtype,typename)VALUES(9,'Супераларм');
INSERT INTO msgtypes (msgtype,typename)VALUES(10,'-');
INSERT INTO msgtypes (msgtype,typename)VALUES(11,'-');
INSERT INTO msgtypes (msgtype,typename)VALUES(12,'-');

-- ТЕСТ
INSERT INTO public.messages(message, host, idst, sourcer, app, msgtype) VALUES ('Создание БД', '127.0.0.1', 0, 14, 11, 10);
INSERT INTO public.messages(message, host, idst, sourcer, app, msgtype, dttm) VALUES ('Тест записи 1', '192.168.0.11', 2, 2, 2, 2, '2017-11-06 16:00');
INSERT INTO public.messages(message, host, idst, sourcer, app, msgtype, dttm) VALUES ('Тест записи 2 ', '192.168.0.11', 3, 3, 3, 3, '2017-11-06 16:00');


