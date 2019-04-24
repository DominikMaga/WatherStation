using System;
using Microsoft.EntityFrameworkCore.Metadata;
using Microsoft.EntityFrameworkCore.Migrations;

namespace Measuring.API.Migrations
{
    public partial class Initial : Migration
    {
        protected override void Up(MigrationBuilder migrationBuilder)
        {
            migrationBuilder.CreateTable(
                name: "Shield",
                columns: table => new
                {
                    ShieldId = table.Column<int>(nullable: false)
                        .Annotation("SqlServer:ValueGenerationStrategy", SqlServerValueGenerationStrategy.IdentityColumn),
                    PhysicalShieldId = table.Column<string>(nullable: true),
                    Name = table.Column<string>(nullable: true),
                    MAC = table.Column<string>(nullable: true),
                    IP = table.Column<string>(nullable: true),
                    Location = table.Column<string>(nullable: true)
                },
                constraints: table =>
                {
                    table.PrimaryKey("PK_Shield", x => x.ShieldId);
                });

            migrationBuilder.CreateTable(
                name: "Sensor",
                columns: table => new
                {
                    SensorId = table.Column<int>(nullable: false)
                        .Annotation("SqlServer:ValueGenerationStrategy", SqlServerValueGenerationStrategy.IdentityColumn),
                    ShieldId = table.Column<int>(nullable: false),
                    PhysicalSensorId = table.Column<string>(nullable: true),
                    Name = table.Column<string>(nullable: true),
                    MeasuredValue = table.Column<string>(nullable: true)
                },
                constraints: table =>
                {
                    table.PrimaryKey("PK_Sensor", x => x.SensorId);
                    table.ForeignKey(
                        name: "FK_Sensor_Shield_ShieldId",
                        column: x => x.ShieldId,
                        principalTable: "Shield",
                        principalColumn: "ShieldId",
                        onDelete: ReferentialAction.Cascade);
                });

            migrationBuilder.CreateTable(
                name: "ShieldConfiguration",
                columns: table => new
                {
                    ShieldConfigurationId = table.Column<int>(nullable: false)
                        .Annotation("SqlServer:ValueGenerationStrategy", SqlServerValueGenerationStrategy.IdentityColumn),
                    ShieldId = table.Column<int>(nullable: false),
                    DeepSleep = table.Column<int>(nullable: false)
                },
                constraints: table =>
                {
                    table.PrimaryKey("PK_ShieldConfiguration", x => x.ShieldConfigurationId);
                    table.ForeignKey(
                        name: "FK_ShieldConfiguration_Shield_ShieldId",
                        column: x => x.ShieldId,
                        principalTable: "Shield",
                        principalColumn: "ShieldId",
                        onDelete: ReferentialAction.Cascade);
                });

            migrationBuilder.CreateTable(
                name: "MeasuredValue",
                columns: table => new
                {
                    MeasuredValueId = table.Column<int>(nullable: false)
                        .Annotation("SqlServer:ValueGenerationStrategy", SqlServerValueGenerationStrategy.IdentityColumn),
                    SensorId = table.Column<int>(nullable: false),
                    Value = table.Column<float>(nullable: false),
                    MeasuredTime = table.Column<DateTime>(nullable: false, defaultValueSql: "getdate()")
                },
                constraints: table =>
                {
                    table.PrimaryKey("PK_MeasuredValue", x => x.MeasuredValueId);
                    table.ForeignKey(
                        name: "FK_MeasuredValue_Sensor_SensorId",
                        column: x => x.SensorId,
                        principalTable: "Sensor",
                        principalColumn: "SensorId",
                        onDelete: ReferentialAction.Cascade);
                });

            migrationBuilder.CreateIndex(
                name: "IX_MeasuredValue_SensorId",
                table: "MeasuredValue",
                column: "SensorId");

            migrationBuilder.CreateIndex(
                name: "IX_Sensor_ShieldId",
                table: "Sensor",
                column: "ShieldId");

            migrationBuilder.CreateIndex(
                name: "IX_ShieldConfiguration_ShieldId",
                table: "ShieldConfiguration",
                column: "ShieldId",
                unique: true);
        }

        protected override void Down(MigrationBuilder migrationBuilder)
        {
            migrationBuilder.DropTable(
                name: "MeasuredValue");

            migrationBuilder.DropTable(
                name: "ShieldConfiguration");

            migrationBuilder.DropTable(
                name: "Sensor");

            migrationBuilder.DropTable(
                name: "Shield");
        }
    }
}
