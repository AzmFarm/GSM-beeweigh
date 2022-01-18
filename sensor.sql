-- phpMyAdmin SQL Dump
-- version 4.9.7
-- https://www.phpmyadmin.net/
--
-- Хост: localhost
-- Время создания: Янв 18 2022 г., 09:05
-- Версия сервера: 5.7.21-20-beget-5.7.21-20-1-log
-- Версия PHP: 5.6.40

SET SQL_MODE = "NO_AUTO_VALUE_ON_ZERO";
SET AUTOCOMMIT = 0;
START TRANSACTION;
SET time_zone = "+00:00";


/*!40101 SET @OLD_CHARACTER_SET_CLIENT=@@CHARACTER_SET_CLIENT */;
/*!40101 SET @OLD_CHARACTER_SET_RESULTS=@@CHARACTER_SET_RESULTS */;
/*!40101 SET @OLD_COLLATION_CONNECTION=@@COLLATION_CONNECTION */;
/*!40101 SET NAMES utf8mb4 */;

--
-- База данных: `Имя_базы_данных`
--

-- --------------------------------------------------------

--
-- Структура таблицы `sensor`
--
-- Создание: Июн 23 2021 г., 09:10
--

DROP TABLE IF EXISTS `sensor`;
CREATE TABLE `sensor` (
  `id` int(6) UNSIGNED NOT NULL,
  `value` decimal(10,2) DEFAULT NULL,
  `beehiveId` int(11) DEFAULT NULL,
  `reading_time` timestamp NOT NULL DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

--
-- Дамп данных таблицы `sensor`
--

INSERT INTO `sensor` (`id`, `value`, `beehiveId`, `reading_time`) VALUES
(625, '43498.77', 1, '2021-06-26 11:41:00'),
(626, '44087.34', 1, '2021-06-26 12:41:03'),
(627, '44365.66', 1, '2021-06-26 13:41:05');

--
-- Индексы сохранённых таблиц
--

--
-- Индексы таблицы `sensor`
--
ALTER TABLE `sensor`
  ADD PRIMARY KEY (`id`);

--
-- AUTO_INCREMENT для сохранённых таблиц
--

--
-- AUTO_INCREMENT для таблицы `sensor`
--
ALTER TABLE `sensor`
  MODIFY `id` int(6) UNSIGNED NOT NULL AUTO_INCREMENT, AUTO_INCREMENT=2774;
COMMIT;

/*!40101 SET CHARACTER_SET_CLIENT=@OLD_CHARACTER_SET_CLIENT */;
/*!40101 SET CHARACTER_SET_RESULTS=@OLD_CHARACTER_SET_RESULTS */;
/*!40101 SET COLLATION_CONNECTION=@OLD_COLLATION_CONNECTION */;
