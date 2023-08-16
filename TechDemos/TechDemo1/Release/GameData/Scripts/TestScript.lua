LogInfo("Running script")

intTest = 2
stringTest = ""

function testFunc()

	LogInfo(stringTest)
	stringTest = "Set in Lua"
	intTest = 5
	
end

function testFuncWithParameters(a)

	LogInfo(a)

end