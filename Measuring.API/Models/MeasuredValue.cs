using System;
using System.ComponentModel.DataAnnotations.Schema;

namespace Measuring.API.Models
{
    [Table("MeasuredValue")]
    public class MeasuredValue
    {
        public int MeasuredValueId { get; set; }
        public int SensorId { get; set; }
        public Sensor Sensor { get; set; }
        public float Value { get; set; }
        public DateTime MeasuredTime { get; set; }
    }
}