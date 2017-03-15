INSERT INTO `migrations` VALUES ('20170213133129');

--Database: Changing position of bugged copper ore in durotar
--issue https://elysium-project.org/bugtracker/issue/11
UPDATE gameobject SET position_x = -6.018759 , position_y = -4227 , position_z = 97.955978 WHERE guid = 4733;
