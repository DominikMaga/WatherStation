using Measuring.API.Models;
using Microsoft.EntityFrameworkCore;

namespace Measuring.API.Data
{
    public class DataContext : DbContext
    {
        public DataContext(DbContextOptions<DataContext> options) : base(options)
        { }

        protected override void OnModelCreating(ModelBuilder modelBuilder)
        {
            modelBuilder.Entity<MeasuredValue>().Property(x => x.MeasuredTime).HasDefaultValueSql("getdate()");
        }

        public DbSet<Shield> Shields { get; set; }
        public DbSet<Sensor> Sensors { get; set; }
        public DbSet<ShieldConfiguration> ShieldConfigurations { get; set; }
        public DbSet<MeasuredValue> MeasuredValues { get; set; }
    }
}