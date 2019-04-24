using System.Collections.Generic;
using System.ComponentModel.DataAnnotations.Schema;

namespace Measuring.API.Models
{
    [Table("Shield")]
    public class Shield
    {
        public int ShieldId { get; set; }
        public string PhysicalShieldId { get; set; }
        public string Name { get; set; }
        public string MAC { get; set; }
        public string IP { get; set; }
        public string Location { get; set; }
        public ICollection<Sensor> Sensors { get; set; }
        public ShieldConfiguration ShieldConfiguration { get; set; }
    }
}