#include "gtest/gtest.h"

#include "lib/EnvimetV4XmlParser.h"

const char* filename = "NewSimulationGebaeude1_AT_08.00.01 23.06.2015.EDX";

TEST(FactorialTest, HandlesZeroInput) {
	EXPECT_EQ(1, 1);
}

class EnvimetV4XmlParserTest : public ::testing::Test
{
protected:
	virtual void SetUp()
	{
		parser = EnvimetV4XmlParser::New();
		parser->SetFileName(filename);
		if(parser->Parse() < 0)
			std::cout << "Error reading file " << filename << std::endl;
	}

	virtual void TearDown()
	{
		parser->Delete();
	}

	EnvimetV4XmlParser* parser;
};

TEST_F(EnvimetV4XmlParserTest, Parse)
{
	EXPECT_EQ(3, parser->SpatialDim);
}
