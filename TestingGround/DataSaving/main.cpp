#include <iostream>
#include <string>
#include <fstream>
#include <ctime>
#include <typeinfo>
#include <vector>
#include <string>

class Item
{
public:
	std::string name;
	int value;

	Item(std::string _name, int _value)
	{
		name = _name;
		value = _value;
	}
};

struct Component
{
	static std::string typeID;
	int value = 7632;

	virtual void Test()
	{
		std::cout << "Test has been called from component!!" << std::endl;
	}
};

struct Transform : public Component
{
	int x = 50, y = 60, z = 5;

	void Test()
	{
		std::cout << "And now transform!!" << std::endl;
	}
};

std::string Component::typeID;

int main()
{
	srand(time(NULL));

	//create a test item
	Item temp("Test01", 50);
	Transform test;
	test.typeID = typeid(test).name();
	test.Test();

	std::string output = "";
	output += "Item type ID: ";
	output += typeid(temp).name();
	std::cout << output <<std::endl; //this function prints out the class name. This can be used in saving data with class types.
	std::cout << "Transform ID: " << typeid(test).name() << std::endl;

	std::ofstream entitySave;
	entitySave.open("entityTest.entity", std::ofstream::out | std::ofstream::trunc);
	entitySave.write((char*)&test, sizeof(test));
	entitySave.close();

	//open a file, clear it and then save the data of the temp item
	std::ofstream dataSave;
	dataSave.open("Test.item", std::ofstream::out | std::ofstream::trunc); // the trunc at the end makes it so the file is cleared then saved to.
	dataSave.write((char*)&temp, sizeof(temp));
	dataSave.close();

	//reset the temp item data to something else
	temp.name = "EMPTY";
	temp.value = rand() % 100;

	std::cout << "Temp data is " << temp.name << " val " << temp.value << std::endl;

	//read the item data from the file and load it into the temp item.
	std::ifstream input;
	input.open("Test.item");
	while (!input.eof())
	{
		input.read((char*)&temp, sizeof(temp));
	}

	std::cout << "Temp data is " << temp.name << " val " << temp.value << std::endl;
}