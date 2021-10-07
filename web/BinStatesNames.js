'use strict';

const BinStatesNames = {
	"states" : {
		"0" : "Выкл. все",
	},
	"buttons" : {
		"0" : "Гостевая комната",
		"1" : "Спальня 1",
		"2" : "Спальня 2",
		"3" : "Детская 1",
		"4" : "Детская 2",
		"5" : "Коридор 2 этаж",
		"6" : "Лестничная кл. 1",
		"7" : "Лестничная кл. 2",
		"8" : "Кабинет",
		"9" : "Кухня 1",
		"10" : "Кухня 2",
		"11" : "Зал 1",
		"12" : "Зал 2",
		"13" : "Зал 3",
		"14" : "Коридор 1 этаж",
		"15" : "Прихожая",
		"16" : "Улица фасад 1",
		"17" : "Улица фасад 2",
		"18" : "Улица двор",
		"19" : "Санузел 2 этаж",
		"20" : "Санузел 1 этаж",
		"21" : "Розетки 1",
		"22" : "Розетки 2",
		"23" : "Выкл. все.",
		"24" : "Я дома!",
		"25" : "Спим!",
		"26" : "Антивор!",
	}
};
const BinStatesGroups = [
		{
			"name" : "Свет осн.",
			"uids" : [0, 127+0, 127+1, 127+2, 127+3, 127+4, 127+5, 127+6, 127+7, 127+8, 127+9, 127+10, 127+11, 127+12, 127+13, 127+14, 127+15, 127+19, 127+20]
		},
		{
			"name": "Улица",
			"uids": [0, 127+16, 127+17, 127+18]
		},
		{
			"name" : "Сценарии",
			"uids" : [0, 127+21, 127+22, 127+23, 127+24, 127+25, 127+26]
		}
]

export { BinStatesNames, BinStatesGroups };
