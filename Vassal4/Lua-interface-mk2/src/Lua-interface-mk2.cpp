#include <Map.h>
#include <Piece.h>
#include <Scripting/Expression.h>
#include <Scripting/ScriptingFactory.h>
#include <Scripting/ScriptResult.h>
#include <Scripting/ScriptingEnvironment.h>
#include <iostream>
#include <list>
#include <string>

using namespace std;

int main() {

	cout << "Vassal-Lua interface Mk2" << endl;

	ScriptingEnvironment *environment = ScriptingFactory::getInstance() -> getEnvironment().get();
	ScriptResult result;

	std::string e2 = //"os.date = 2;\n"
				//"os.date=10\n"
				"print ()\n"
						"print ('-------------------------------------------------------------------')\n"
						"print('Test 1.1:  Reads to the environment are re-directed to the copy in _V.env')\n"
						"print('Time='..os.time()..', Test succeeded!')\n"
						"print ()\n"
						"print ('-------------------------------------------------------------------')\n"

						"print('Test 1.2: Allow rawget on local table')\n"
						"local tab = {11, 22, 33}\n"
						"print('Success! Should print 22='..rawget (tab, 2))\n"
						"print()\n"

						"print('Test 1.3: Allow rawset on local table')\n"
						"local tab = {11, 22, 33}\n"
						"rawset(tab, 2, 42)\n"
						"print('Success! Should print 42='..tab[2])\n"
						"print()\n"

						"print('Test 1.4: Allow getmetatable on local table')\n"
						"setmetatable(tab, {})\n"
						"print('Success!')\n"
						"print()\n"

						"print('Test 1.5: Allow getmetatable on local table')\n"
						"local t5 = getmetatable(tab)\n"
						"print('Success!')\n"
						"print()\n"

						"print('The following tests should generate fatal errors')\n"
						"print ()\n"
						"print ('-------------------------------------------------------------------')\n"

						"print('Test 2.1: Disallow setmetatable on environment table')\n"
						"local t21 = _ENV; setmetatable(t21, nil)\n"
						"print('Test failed!')\n"
						"print ()\n"
						"print ('-------------------------------------------------------------------')\n"

	//					"print('Test 2.2: Disallow getmetatable on environment table')\n"
	//					"local t22 = _ENV; local t222 = getmetatable(t22)\n"
	//					"print('Test failed!')\n"
	//					"print ()\n"
	//					"print ('-------------------------------------------------------------------')\n"
	//
	//					"print('Test 2.3: Disallow rawset on environment table')\n"
	//					"local x = _ENV; rawset(x, 'Hi', 'there')\n"
	//					"print('Test failed!, new value set in ENV='..Hi)\n"
	//					"print ()\n"
	//					"print ('-------------------------------------------------------------------')\n"
	//
	//					"print('Test 2.4: Generate error on rawget on environment table')\n"
	//					"local t5 = rawget(_ENV, os)\n"
	//					"print('Test failed!')\n"
	//					"print ()\n"
	//					"print ('-------------------------------------------------------------------')\n"
	//
	//					"print('Test 2.5: Generate error if trying to access undefined global (or redirect to a Vassal global)?')\n"
	//					"print(x)"
	//					"print('Test failed!')\n"
	//					"print ()\n"
	//					"print ('-------------------------------------------------------------------')\n"
//
//						"print('Test 2.6: Read-only (no update of existing global variable)')\n"
//						"__lock = nil\n"
//						"print('Test failed!')\n"
//
//						"print('Test 2.6: Read-only (no creation of new global variable)')\n"
//						"x=10\n"
//						"print('Test failed!')\n"
//
//						"x = 10\n"
//						"print ('x='..x)\n"
//						"print ('os.date='..tostring(os.date));\n"
//						"print 'call this:getProperty' ;\n"
//						"print('Name='..this:getProperty('Name'))";
;
		Script exp2(environment, e2, "SandboxTest");

		Piece piece ("Uchi-mata", 4);
		// cout << "Piece is at "<< &piece << endl;

		Map map ("Lothlorien");
		// cout << "Map is at "<< &map << endl;

		piece.setMap(&map);

//		bool j = exp2.execute(piece);
//
//		if (j) {
//			cout << "Script ran successfully" << endl;
//		} else {
//			cout << "Script failed, error message=" << exp2.getError() << endl << endl;
//		}


		// Expression exp1("this:getMap():getProperty('Name')", "Simple Expression");
		std::string s1 =
				"print ('>>Start')\n"
				//"\n"
				//"local function fib(n) return n<2 and n or fib(n-1)+fib(n-2) end\n"
				"local startClock, startTime = os.clock(), os.time()\n"
				//"\n"
				"local x = this.flipx\n"
				"print('x='..tostring(x))"
				//"setmetatable(this, {})"
				//"this.junk = 10"
				//"print ('this:getName()='..tostring(this:getName()))"
				//"local s = '123456789012345'\n"
				//"for i = 1, 36 do "
				//"print ('i='..i)"
				//"s = s .. s "
				//"end\n"
				//" local x = 0"
				//" for i = 1, 10000 do\n"
				//"   for j = 1, 10000 do\n"
				//"		x = x + 1"
				//"   end\n"
				//"end\n"
				//"string.rep(string.rep(string.rep(string.rep(string.rep('a', 40), 40), 40),40),40)\n"
				// "local s = '01234567890123456789012345678901234567890123456789'\n"
				// "s:find('.*.*.*.*.*x')\n"
				//"print('(x):rep(200)='..('x'):rep(200))\n"
				//"print(string.rep('x', 100))\n"
				//"for x in this:getMap():getVisiblePieces() do\n"
				//"	print ('>>x='..tostring(x:getName()))\n"
				//"end\n"
				//"print ('this.Name='..this.Name)\n"
				//"local x = this.ptr; print(tostring(x()))\n"
				//"this.Name = 'me'\n"
				//"print('fib(37)='..fib(37))\n"
				"local endClock = os.clock()\n"
				"local endTime = os.time()\n"
				"print ('Run time = '..tostring(os.difftime(endTime, startTime))..'s')\n"
				"print ('CPU time = '..tostring(endClock-startClock)..'s')\n"
				//"print (inc(1))\n"
				//"print('---Piece name from this:getProperty(Name) = '..this:getProperty('Name'))\n"
				//"local m = this:getMap()\n"
				//"print('---Script 1: Map Name='..m:getName())\n"
				//"print('---Script 1: Map name property='..m:getProperty('Name'))";
		;
		Script exp1 (environment, s1, "MapTestUpper");
		//Expression exp1 ("this:getProperty('Name')", "thisTest");


		exp1.execute(&piece, result);

		if (result.isSuccessful()) {
			cout << "Script ran successfully: " + result.getResult()->toString() << endl;
		} else if (result.isScriptError()) {
			cout << "Script Error: " << result.getError() << endl << endl;
		} else if (result.isVassalError()) {
			cout << "Vassal Error: " << result.getError() << endl << endl;
		}



//		std::list<Piece> pieces;
//		Piece *p = new Piece ("Piece 1", 1);
//		pieces.push_back(*p);
//		p = new Piece ("Piece 2", 2);
//		pieces.push_back(*p);

	cout << "Vassal-Lua interface Mk2: Complete" << endl;
	return 0;
}
