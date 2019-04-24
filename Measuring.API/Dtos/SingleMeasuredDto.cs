namespace Measuring.API.Dtos
{
    public class SingleMeasuredDto
    {
        public string PhysicalShieldId { get; set; }
        public string PhysicalSensorId { get; set; }
        public float Value { get; set; }
    }
}