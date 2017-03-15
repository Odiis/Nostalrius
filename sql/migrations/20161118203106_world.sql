INSERT INTO `migrations` VALUES ('20161118203106'); 

-- In Loving Memory: Rousch missing emotes
INSERT INTO `creature_ai_scripts` VALUES
(1609001, 16090, 1, 0, 100, 1, 0, 5, 70000, 70000, 47, 8, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 'Rousch - /kneel'),
(1609002, 16090, 1, 0, 100, 1, 60000, 60000, 70000, 70000, 47, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 'Rousch - /stand'),
(1609003, 16090, 1, 0, 100, 1, 61000, 61000, 71000, 71000, 5, 66, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 'Rousch - /salute');

UPDATE `creature_template` SET `AIName` = 'EventAI' WHERE `entry` = 16090;