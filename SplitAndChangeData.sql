#By Nroskill 2017/04/26
#MySQL
#来自Gizeta的奇怪需求
#将enemy为100,200,-1,200这样的VARCHAR
#需要将其中的每个数字除-1和最后一个外，加1000

#此处改库名
USE `userdb`;

DROP FUNCTION IF EXISTS `SPLIT_STR`;
DROP PROCEDURE IF EXISTS `shit`;
#抄自 http://www.cnblogs.com/qiaoyihang/p/6270165.html
CREATE FUNCTION `SPLIT_STR`(
  x VARCHAR(255),
  delim VARCHAR(12),
  pos INT
)
RETURNS VARCHAR(255)
RETURN REPLACE(SUBSTRING(SUBSTRING_INDEX(x, delim, pos),
       LENGTH(SUBSTRING_INDEX(x, delim, pos -1)) + 1),
       delim, '');
DELIMITER |
CREATE PROCEDURE `shit`()
BEGIN
    DECLARE done INT DEFAULT 0;
    DECLARE len INT DEFAULT 0;
    DECLARE t INT DEFAULT 0;
    DECLARE num INT DEFAULT 0;
    DECLARE i INT DEFAULT 0;
    DECLARE e VARCHAR(70) DEFAULT '';
    DECLARE result VARCHAR(70) DEFAULT '';
    #此处改表和字段名
    DECLARE cur CURSOR FOR SELECT `id`, `enemy` FROM `student`;
    DECLARE CONTINUE HANDLER FOR NOT FOUND SET done := 1;
    OPEN cur;
    FETCH cur INTO i, e;
    WHILE done = 0 DO
        SET e := REPLACE(e, ' ', '');
        SET len := LENGTH(e) - LENGTH(REPLACE(e, ',', ''));
        SET t := 1;
        SET result := '';
        WHILE t <= len DO
            SET num := CAST(SPLIT_STR(e, ',', t) AS SIGNED);
            IF num != -1 THEN
                SET num := num + 1000;
            END IF;
            SET result := CONCAT(result, CAST(num AS CHAR));
            SET result := CONCAT(result, ',');
            SET t = t + 1;
        END WHILE;
        SET result := CONCAT(result, SPLIT_STR(e, ',', len + 1));
        #此处改表和字段名
        UPDATE `student` SET `enemy` = result WHERE `id` = i;
        FETCH cur INTO i, e;
    END WHILE;
    CLOSE cur;
END
|
DELIMITER ;
START TRANSACTION;
CALL shit();
COMMIT;
DROP FUNCTION IF EXISTS `SPLIT_STR`;
DROP PROCEDURE IF EXISTS `shit`;