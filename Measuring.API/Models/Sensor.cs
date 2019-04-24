using System.Collections.Generic;
using System.ComponentModel.DataAnnotations.Schema;

namespace Measuring.API.Models
{
    [Table("Sensor")]
    public class Sensor
    {
        public int SensorId { get; set; }
        public int ShieldId { get; set; }
        public Shield Shield { get; set; }
        public string PhysicalSensorId { get; set; }
        public string Name { get; set; }
        public string MeasuredValue { get; set; }
    }
}