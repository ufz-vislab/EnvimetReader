#include "gtest/gtest.h"

#include "lib/EnvimetV4XmlParser.h"

#include <vtkFloatArray.h>
#include <vtkStringArray.h>

const char* filename = "NewSimulationGebaeude1_AT_08.00.01 23.06.2015.EDX";

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
	double abs_error = 0.00001;
	EXPECT_EQ(3, parser->NumberOfSpatialDimensions);
	EXPECT_EQ(100, parser->XDimension);
	EXPECT_EQ(100, parser->YDimension);
	EXPECT_EQ(30, parser->ZDimension);

	EXPECT_EQ(parser->XDimension, parser->XSpacing->GetNumberOfTuples());
	EXPECT_EQ(parser->YDimension, parser->YSpacing->GetNumberOfTuples());
	EXPECT_EQ(parser->ZDimension, parser->ZSpacing->GetNumberOfTuples());

	EXPECT_EQ(1.0, parser->XSpacing->GetTuple1(0));
	EXPECT_EQ(1.0, parser->YSpacing->GetTuple1(0));
	ASSERT_NEAR(164.84485, parser->ZSpacing->GetTuple1(parser->ZSpacing->GetNumberOfTuples() - 1), abs_error);

	EXPECT_EQ(36, parser->VariableNames->GetNumberOfValues());
	EXPECT_EQ("Objects ( )", parser->VariableNames->GetValue(0));
	EXPECT_EQ("Div Rlw Temp change (K/h)", parser->VariableNames->GetValue(parser->VariableNames->GetNumberOfValues() - 1));

	EXPECT_EQ("NewSimulationGebaeude1_AT_", parser->SimulationBaseName);
	EXPECT_EQ("23.06.2015", parser->SimulationDate);
	EXPECT_EQ("08:00:01", parser->SimulationTime);
	EXPECT_EQ("ufz Gebaeude 1", parser->ProjectName);
	ASSERT_NEAR(-4.4f, parser->ModelRotation, abs_error);
	ASSERT_NEAR(0.0, parser->LocationGeorefX, abs_error);
	ASSERT_NEAR(0.0, parser->LocationGeorefY, abs_error);
}
