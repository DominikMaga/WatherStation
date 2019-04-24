using System.ComponentModel.DataAnnotations.Schema;

namespace Measuring.API.Models
{
    [Table("ShieldConfiguration")]
    public class ShieldConfiguration
    {
        public int ShieldConfigurationId { get; set; }
        public int ShieldId { get; set; }
        public Shield Shield { get; set; }
        public int DeepSleep { get; set; }
    }
}